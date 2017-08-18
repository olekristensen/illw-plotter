// QRZ JavaScript Library, Copyright 2015, N5JLC

var qrz = {
  _baseXmlUrl: '//xmldata.qrz.com/xml/1.33/?',
  _getXml: function (url, onResponse, params) {
    var xhttp = new XMLHttpRequest()
    xhttp.onreadystatechange = function () {
      if (xhttp.readyState == 4 && xhttp.status == 200) {
        params.responseXML = xhttp.responseXML
        onResponse(params)
      }
    }
    xhttp.open('GET', url, true)
    xhttp.send()
  },
  _getXmlValue: function (xmlDocument, nodeName) {
    var rtn = ''

    try {
      rtn = xmlDocument.getElementsByTagName(nodeName)[0].childNodes[0].nodeValue
    } catch (ex) {
      if (ex.name === 'TypeError') {
                // Just means this property isn't available for this user.
        rtn = ''
      } else {
        console.log('_getXmlValue exception: ' + ex)
        console.log('nodeName = ' + nodeName)
      }
    }

    return rtn
  },
  get key () {
    var cname = 'qrzKey'
    var name = cname + '='
    var ca = document.cookie.split(';')
    for (var i = 0; i < ca.length; i++) {
      var c = ca[i]
      while (c.charAt(0) == ' ') c = c.substring(1)
      if (c.indexOf(name) == 0) return c.substring(name.length, c.length)
    }
    return ''
  },
  set key (strKey) {
    var cname = 'qrzKey'
    var cvalue = strKey
    var exdays = 7

    var d = new Date()
    d.setTime(d.getTime() + (exdays * 24 * 60 * 60 * 1000))
    var expires = 'expires=' + d.toUTCString()
    document.cookie = cname + '=' + cvalue + '; ' + expires
  },
  userProperties: ['call', 'xref', 'aliases', 'dxcc', 'fname', 'name', 'addr1', 'addr2', 'state', 'zip', 'country', 'ccode', 'lat', 'lon', 'grid', 'county', 'fips', 'land', 'efdate', 'expdate', 'p_call', 'class', 'codes', 'qslmgr', 'email', 'url', 'u_views', 'bio', 'biodate', 'image', 'imageinfo', 'serial', 'moddate', 'MSA', 'AreaCode', 'TimeZone', 'GMTOffset', 'DST', 'eqsl', 'mqsl', 'cqzone', 'ituzone', 'born', 'user', 'lotw', 'iota', 'geoloc'],
  login: function (params) {
        // Possible params: username, password, responseXML
        // Example: qrz.login({ username: "foo", password: "bar" });
    if (params.responseXML) {
      qrz.key = params.responseXML.getElementsByTagName('Key')[0].childNodes[0].nodeValue
    } else if (params.username && params.password) {
      qrz._getXml(this._baseXmlUrl + 'username=' + params.username + ';password=' + encodeURIComponent(params.password), this.login, {})
    } else {
      console.log('Username and password must be provided to login.')
    }
  },
  query: function (params) { // Possible params: operator, responseXML, finalCallback
    if (params.responseXML) {
      if (qrz._getXmlValue(params.responseXML, 'Callsign')) {
        if (params.operator) {
          var op = params.operator
                    // fill it
          for (var i = 0; i < qrz.userProperties.length; i++) {
            op[qrz.userProperties[i]] = qrz._getXmlValue(params.responseXML, qrz.userProperties[i])
          }

          params.finalCallback(op)
        } else {
          console.log("Operator didn't return from call to getXml.")
        }
      } else {
        if (params.operator) {
          var op = params.operator
          params.finalCallback(op)
        }
        // Perform error handling from XML data
        console.log('Unhandled exception happened.')
      }
    } else if (params.operator) {
      if (qrz.key == '') {
        console.log('User is not logged in.')
      } else {
        qrz._getXml(this._baseXmlUrl + 's=' + qrz.key + ';callsign=' + params.operator.call, qrz.query, {
          'operator': params.operator,
          'finalCallback': params.finalCallback
        })
      }
    }
  }
}

function Operator (callsign) {
  for (var i = 0; i < qrz.userProperties.length; i++) {
    this[qrz.userProperties[i]] = ''
  }

  this.call = callsign
  this._bioHtml = ''

  this.getBioHtml = function (callbackFunction) {
    if (this._bioHtml != '') {
            // Already been loaded
      callbackFunction(this._bioHtml)
    } else if (this.bio != '') {
            // Bio is available, but not loaded.  Go get it.
      try {
        var xhttp = new XMLHttpRequest()
        xhttp.onreadystatechange = function () {
          if (xhttp.readyState == 4 && xhttp.status == 200) {
            this._bioHtml = xhttp.responseText
            callbackFunction(xhttp.responseText)
          }
        }
        xhttp.open('GET', qrz._baseXmlUrl + 's=' + qrz.key + ';html=' + this.call, true)
        xhttp.send()
      } catch (ex) {
                // Can't get the bio. Don't try again.
        this.bio = ''
        callbackFunction('')
      }
    } else {
            // Bio isn't available for this person
      callbackFunction('')
    }
  }

  this.load = function (callbackFunction) {
    qrz.query({
      'operator': this,
      'finalCallback': callbackFunction
    })
  }
}

function operator_loaded (operator) {
  console.log('FINAL CALLBACK: operator_loaded...')
  console.log(operator)
}

function safe_tags (str) {
  return str.replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;')
}

function logOperator (dataKey, dataObj) {
  if (!dataObj.logged) {
    if (!dataObj.gridsquare && dataObj.qrz.grid) {
      dataObj['gridsquare'] = dataObj.qrz.grid
    }

    var c = {
      lat: 0,
      lon: 0
    }

    if (dataObj.qrz && dataObj.qrz.lat && dataObj.qrz.lon) {
      var qrzLat = parseFloat(dataObj.qrz.lat)
      var qrzLon = parseFloat(dataObj.qrz.lon)
      c.lat = qrzLat
      c.lon = qrzLon

      var tokens = dataObj.qrz.addr2.split(/[\s,\n\.]+/).filter(function (t) { return t != '' }),
        res = [],
        i,
        len,
        component
      for (i = 0, len = tokens.length; i < len; i++) {
        component = tokens[i]
        res.push(component.substring(0, 1).toUpperCase())
        res.push(component.substring(1).toLowerCase())
        res.push(' ') // put space back in
      }
      var newLocation = {
        'illw': dataObj.call,
        'name': res.join(''),
        'country': dataObj.qrz.country,
        'dxcc': dataObj.dxcc,
        'continent': '',
        'coordinate': c
      }

      addLogEntry(newLocation, $('#locator-search .btn-add-log-entry'))
      markLogged(dataKey, dataObj)
    } else if (dataObj.gridsquare) {
      var value = dataObj.gridsquare

      // make sure the capitalisation and numbering scheme is consistent
      var filteredValue = ''
      for (var i = 0; i < value.length; i++) {
        if (i >= 0 && i < 2) {
          filteredValue += value.charAt(i).toUpperCase()
        } else if (i < 4) {
            /*            if (!isNaN(value.charAt(i))) {
             filteredValue += parseInt(value.charAt(i))
             }
             */
          filteredValue += value.charAt(i)
        } else if (i < 6) {
          filteredValue += value.charAt(i).toLowerCase()
        } else {
          filteredValue += value.charAt(i)
        }
      }
      if (value != filteredValue) {
        value = filteredValue
      }
      $('#locatorField').val(value)

    // try converting to decimal coordinates
      try {
        c = HamGridSquare.toLatLon(value, c)

        // find the size of the grid square

        var level = Math.floor(value.length / 2.0)
        var size

        if (level == 1) {
          size = {
            lat: 10,
            lon: 20
          }
        } else if (level == 2) {
          size = {
            lat: 1,
            lon: 2
          }
        } else if (level == 3) {
          size = {
            lat: (1.0 / 60.0) * 2.5,
            lon: (1.0 / 60.0) * 5
          }
        }

        // try retrieving a name from google's reverse geocoding

        var geocoder = new google.maps.Geocoder()

        geocoder.geocode({
          'location': {
            'lat': c.lat,
            'lng': c.lon
          }
        }, function (results, status) {
          var name = value
          var country = ''
          var dxcc = ''

          if (status === 'OK') {
            if (results[1]) {
              var divisor = 2 * level

                    // search for a geometry that contains our reduced square

              var matches = 0

              for (var i = 0; i < results.length; i++) {
                var r = results[i]
                if ((i == results.length - 1) || (r.geometry.viewport.contains({
                  'lat': (c.lat - (size.lat / divisor)),
                  'lng': (c.lon - (size.lon / divisor))
                }) && r.geometry.viewport.contains({
                  'lat': (c.lat + (size.lat / divisor)),
                  'lng': (c.lon + (size.lon / divisor))
                }))) {
                  for (var j = 0; j < r.address_components.length; j++) {
                    var addressType = r.address_components[j].types[0]
                    if (addressType == 'country') {
                      dxcc = r.address_components[j].short_name
                      country = r.address_components[j].long_name
                      if ((i == results.length - 1) && matches == 0 && j == 0) {
                        name = name + ', ' + r.address_components[j].long_name
                      }
                    } else {
                      matches++
                      name = name + ', ' + r.address_components[j].long_name
                    }
                    break
                  }
                }
              }

              var newLocation = {
                'illw': dataObj.call,
                'name': name,
                'country': country,
                'dxcc': dataObj.dxcc,
                'continent': '',
                'coordinate': c
              }
              addLogEntry(newLocation, $('#locator-search .btn-add-log-entry'))
              markLogged(dataKey, dataObj)
            } else {
                    // empty results from google
              var newLocation = {
                'illw': dataObj.call,
                'name': name,
                'country': country,
                'dxcc': dataObj.dxcc,
                'continent': '',
                'coordinate': c
              }
              addLogEntry(newLocation, $('#locator-search .btn-add-log-entry'))
              markLogged(dataKey, dataObj)
            }
          } else {
                // geocoding failed getting results from google, lets look for oceans
            $.ajax({
              type: 'GET',
              url: 'http://api.geonames.org/extendedFindNearby?lat=' + c.lat + '&lng=' + c.lon + '&username=olekristensen',
              cache: true,
              dataType: 'xml',
              success: function (xml) {
                console.log(xml)
                $(xml).find('geonames').each(function () {
                  $(this).find('ocean').each(function () {
                    var name = $(this).find('name').text()
                    var newLocation = {
                      'illw': dataObj.call,
                      'name': value + ', ' + name,
                      'country': '',
                      'dxcc': dataObj.dxcc,
                      'continent': '',
                      'coordinate': c
                    }
                    addLogEntry(newLocation, $('#locator-search .btn-add-log-entry'))
                    markLogged(dataKey, dataObj)
                  })
                })
              },
              error: function (e) {
                var newLocation = {
                  'illw': dataObj.call,
                  'name': value,
                  'country': '',
                  'dxcc': dataObj.dxcc,
                  'continent': '',
                  'coordinate': c
                }
                addLogEntry(newLocation, $('#locator-search .btn-add-log-entry'))
                markLogged(dataKey, dataObj)
              }
            })
          }
        })
      } catch (e) {
        // grid locator failed
        alert('failed to locate')
      }
    } else if (dataObj.dxcc) {
      // lookup in

    }
  }
}

var markLogged = function (dataKey, dataObj) {
  if (!dataObj.logged) {
    $('#' + dataKey + ' .glyphicon-ok').show()
    firebase.database().ref('log/' + dataKey).update({'logged': true})
  }
}

function init () {
    // Initialize Firebase
  var config = {
    apiKey: 'AIzaSyDFvU7lkuciYy5z4FHoNii_z93PBqEuSIA',
    authDomain: 'illw-b0c15.firebaseapp.com',
    databaseURL: 'https://illw-b0c15.firebaseio.com',
    projectId: 'illw-b0c15',
    storageBucket: 'illw-b0c15.appspot.com',
    messagingSenderId: '395178732063'
  }
  firebase.initializeApp(config)

  var parseLine = function (line) {
    var record = {}
    var fields = line.split('<')

    record['adif'] = line

    for (var i = 0; i < fields.length; i++) {
      var field = fields[i],
        fieldName = field.split(':')[0],
        fieldValue = field.split('>')[1]

      if (fieldName.length && fieldName != 'EOR>' && fieldValue && fieldValue.length) {
        fieldName = fieldName.trim().toLowerCase()
        fieldValue = fieldValue.trim()
        record[fieldName] = fieldValue
      }
    }
    if (record.qso_date) {
      var qso_date = new Date(Date.UTC(record.qso_date.slice(0, 4),
                (record.qso_date.slice(4, 6) - 1),
                record.qso_date.slice(6, 8),
                record.time_on.slice(0, 2),
                record.time_on.slice(2, 4),
                record.time_on.slice(4)))
      record.qso_date = qso_date
      delete record['time_on']
      delete record['time_off']
    }
    return record
  }

  var commentsRef = firebase.database().ref('log')
  commentsRef.on('child_added', function (data) {
    var dataObj = data.val()
    var dataKey = data.key

    if (!dataObj.call) {
      var updates = {}
      dataObj = parseLine(dataObj.adif)
      updates['/log/' + data.key] = dataObj
      firebase.database().ref().update(updates)
    }

    if (!dataObj.qrz) {
      var op = new Operator(dataObj.call)
      op.load(function (operator) {
        console.log(operator)
        var o = {}
        for (var i = 0; i < qrz.userProperties.length; i++) {
          o[qrz.userProperties[i]] = op[qrz.userProperties[i]]
        }
        dataObj[ 'qrz' ] = o
        firebase.database().ref('/log/' + data.key).update({
          'qrz': o
        })
        logOperator(dataKey, dataObj)
        op.getBioHtml(function (bio) {
          if (bio != '') {
            $('#qrz-bio .location-selection').html(
              '<h4>' + dataObj.call + '</h4>' + (dataObj.image ? '<img src="' + dataObj.image + '" class="media-object img-rounded pull-right" style="height:100px">' : '') + $(bio).find('#biodata').html())
            $('#qrz-bio .location-selection-container').slideDown(500)
          } else {
            $('#qrz-bio .location-selection-container').slideUp(500)
          }
        })
      })
    } else {
      logOperator(dataKey, dataObj)
    }

    var $newRows = $('#firebase >tbody:last-child').append(
            '<tr style="display:none;" class="newRow' + ' success' + '" id="' + data.key + '">' +
            '<td>' + '<span class="glyphicon glyphicon-ok" aria-hidden="true" style="display:' + (dataObj.logged ? 'inherit' : 'none') + '"></span>' + '</td>' +
            '<td><samp>' + dataObj.call + '</samp></td>' +
            '<td><samp>' + dataObj.dxcc + '</samp></td>' +
            '<td><samp>' + (dataObj.gridsquare ? dataObj.gridsquare : '') + '</samp></td>' +
            '<td class="data"><small><samp>' + safe_tags(dataObj.adif) + '</samp></small></td>' +
            '</tr>')
    $('#firebase >tbody tr.newRow').fadeIn(500, function () {
      $('#firebase >tbody tr.newRow').removeClass('newRow')
      setTimeout(function () {
        $('#firebase >tbody tr.success').removeClass('success')
      }, 1000)
    })
    $('#firebase > tbody').scrollTop($('#firebase > tbody')[0].scrollHeight)
    formatFirebaseTable()
  })
}

var loggingIn = false

function checkLogin (callback) {
  if (qrz.key == '') {
    if (!loggingIn) {
      warningAlert('QRZ login in progress')
      loggingIn = true
      qrz.login({ username: 'OZ2TEST', password: 'Ikw#4Wtx.' })
    }
    setTimeout(function () { checkLogin(callback) }, 100)
  } else {
    loggingIn = false
    successAlert('QRZ login sucessfull')
    callback()
  }
}

$(document).ready(function () {
  checkLogin(init)
})
