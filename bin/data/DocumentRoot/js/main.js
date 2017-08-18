// =============================================================================
//
// Copyright (c) 2009-2013 Christopher Baker <http://christopherbaker.net>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// =============================================================================

var JSONRPCClient; // /< The core JSONRPC WebSocket client.

var initialOffset = null;

var selectedLocation;

var locationTerms;

var googleMapsApi = 'AIzaSyD7GFqO4gfF7J9ycptEaCNLLGU6i3TnaQ8';

$.fn.rotateElement = function(angle) {
    var $elementToRotate = this,
        deg = angle,
        deg2radians = Math.PI * 2 / 360,
        rad = deg * deg2radians,
        costheta = Math.cos(rad),
        sintheta = Math.sin(rad),

        m11 = costheta,
        m12 = -sintheta,
        m21 = sintheta,
        m22 = costheta,
        matrixValues = 'M11=' + m11 + ', M12=' + m12 + ', M21=' + m21 + ', M22=' + m22;

    $elementToRotate.css('-webkit-transform', 'rotate(' + deg + 'deg)')
        .css('-moz-transform', 'rotate(' + deg + 'deg)')
        .css('-ms-transform', 'rotate(' + deg + 'deg)')
        .css('transform', 'rotate(' + deg + 'deg)')
        .css('filter', 'progid:DXImageTransform.Microsoft.Matrix(sizingMethod=\'auto expand\',' + matrixValues + ')')
        .css('-ms-filter', 'progid:DXImageTransform.Microsoft.Matrix(SizingMethod=\'auto expand\',' + matrixValues + ')');
    return $elementToRotate;
};

function updateCompassArrow(obj) {
    $('#compassDiscArrow').rotateElement(obj.bearing)
    $('#distance').html('<h2><span class="label label-default">' + Math.round(obj.distance, 2) + ' km</span></h2><p class="lead"><strong>' + obj.destination.illw + '</strong> ' + obj.destination.name + '<br/>' + obj.destination.country + '</p>')
}

function addError(error) {
    console.log(error)
}

function onWebSocketOpen(ws) {
    console.log('on open')
    console.log(ws)
}

function onWebSocketMessage(evt) {
    if (evt.data == 'refreshLog') {
        refreshLog()
    }
    console.log('on message:')
    console.log(evt.data)
}

function onWebSocketClose() {
    console.log('on close')
}

function onWebSocketError() {
    console.log('on error')
}

function addLogEntry(location, v, callbackFunction) {
    var $this = $(this)
    JSONRPCClient.call('addLogEntry',
        location,
        function(result) {
            if (result != null) {
                successAlert('<strong>' + result.destination.illw + '</strong> ' + result.destination.name + ' has been logged.')
                callbackFunction(v)
            }
        },
        function(error) {
            addError(error)
        })
}

function hideSelection(v) {
    $(v).parent().parent().parent().find('.location-selection-container').slideUp(500)
}

function successAlert(message) {
    $('#alert').addClass('alert-success')
    $('#alert').removeClass('alert-warning')
    alert(message)
}

function warningAlert(message) {
    $('#alert').addClass('alert-warning')
    $('#alert').removeClass('alert-success')
    alert(message)
}

function alert(message) {
    $('#alert').html(message)
    $('#alert').fadeTo(2000, 500, function() {
        $('#alert').fadeTo(2000, 0)
    })
}

function refreshLog() {
    var $this = $(this)
    var count = $('#log >tbody >tr').length
    console.log(count)
    JSONRPCClient.call('getLog',
        count,
        function(result) {
            console.log(result)
            if (result.length > 0) {
                $.each(result, function(idx, obj) {
                    var $newRows = $('#log >tbody:last-child').append(
                        '<tr style="display:none;" class="newRow' + (count > 0 ? ' success' : '') + '">' +
                        '<td nowrap>' + obj.timestamp + '</td>' +
                        '<td>' + obj.bearing + '</td>' +
                        '<td>' + obj.distance + '</td>' +
                        '<td><strong>' + obj.destination.illw + '</strong></td>' +
                        '<td>' + obj.destination.country + '</td>' +
                        '<td>' + obj.destination.name + '</td>' +
                        '</tr>')
                    updateCompassArrow(obj)
                    addMapMarker(obj)

                })
                $('#log >tbody tr.newRow').fadeIn(500, function() {
                    $('#log >tbody tr.newRow').removeClass('newRow')
                    setTimeout(function() {
                        $('#log >tbody tr.success').removeClass('success')
                    }, 1000)
                })
                formatLogTable()
                d = new Date();
                $('#plot').attr("src", "/img/latest-plot.svg?" + d.getTime());
                $('#log > tbody').scrollTop($('#log > tbody')[0].scrollHeight)
            }
        },
        function(error) {
            addError(error)
        })
}

function addMapMarker(obj){
  if($('#map').length){
    var latLng = new google.maps.LatLng(obj.destination.coordinate.lat,obj.destination.coordinate.lon);
    var marker = new google.maps.Marker({
      position: latLng,
      map: map,
      icon: {
        path: google.maps.SymbolPath.CIRCLE,
        scale: 5,
        fillColor: 'yellow',
        fillOpacity: 0.3,
        strokeColor: 'yellow',
        strokeWeight: 2
      },
      title: obj.destination.illw,
      animation: google.maps.Animation.DROP
    });
    map.setCenter(latLng)
  }
}

function initializeButtons() {
    $('.btn-add-log-entry').each(function(i, v) {
        $(v).on('click', function() {
            addLogEntry(selectedLocation, v, function(v){
              hideSelection(v)
              $(v).parent().parent().parent().find('input').val('')
              $(v).parent().parent().parent().find('.location-search').val('').trigger('change')
              $(v).parent().parent().parent().find('.location-search').select2('open')
              $(v).parent().parent().parent().find('input').focus()
            } )
        })
    })

    $('#refresh-log').on('click', function() {
        refreshLog()
    })
}

function selectLocation(location, container, focus, zoom = 13) {
    var markup = "<div class='media'>" +
        "<div class='pull-right'>" +
        "<img class='media-object img-rounded' src='https://maps.googleapis.com/maps/api/staticmap?maptype=terrain&center="
    if (container.is($('#locator-search'))) {
        var level = Math.floor($('#locatorField').val().length / 2.0)
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

        var c = location.coordinate

        markup += c.lat + ',' + c.lon + '&path=color:0xFFBE31ff|weight:2|fillcolor:0xFFBE3166|geodesic:true|' + (c.lat - (size.lat / 2.0)) + ',' + (c.lon - (size.lon / 2.0)) + '|' + (c.lat - (size.lat / 2.0)) + ',' + (c.lon + (size.lon / 2.0)) + '|' + (c.lat + (size.lat / 2.0)) + ',' + (c.lon + (size.lon / 2.0)) + '|' + (c.lat + (size.lat / 2.0)) + ',' + (c.lon - (size.lon / 2.0)) + '|' + (c.lat - (size.lat / 2.0)) + ',' + (c.lon - (size.lon / 2.0))
    } else {
        markup += location.coordinate.lat + ',' + location.coordinate.lon + '&markers=color:0xFFBE31FF%7Csize:small%7C' + location.coordinate.lat + ',' + location.coordinate.lon + '&zoom=' + zoom
    }

    markup += '&style=element:geometry.stroke|visibility:off&style=feature:landscape|element:geometry|saturation:-100&style=feature:water|saturation:-100|invert_lightness:true&size=400x120&key=' + googleMapsApi + "'/>" +
        '</div>' +
        "<div class='media-body'><b>" + location.illw + '</b> ' + location.name +
        '<br><small>' + location.country + '</small> ' +
        '<br><small>' + location.continent + '</small> ' +
        '<br><small>' + location.dxcc + '</small> ' +
        '<br><small>' + location.coordinate.lat + '</small> ' +
        '<br><small>' + location.coordinate.lon + '</small> ' +
        '</div>' +
        '</div>'

    container.find('.location-selection').html(markup)
    container.find('.location-selection-container').slideDown(500)
    selectedLocation = location
    if (focus) container.find('.btn-add-log-entry').focus()
}

function formatLocation(location) {
    if (location.loading) return 'Fetching results from server...'

    var tokens = locationTerms.trim().split(' ')
    var exp = tokens.join('|');
    var re = new RegExp(exp, 'gi')

    var markup = "<div class='media'>" +
        "<div class='pull-right'>" +
        "<img class='media-object img-rounded' src='https://maps.googleapis.com/maps/api/staticmap?maptype=terrain&center=" + location.coordinate.lat + ',' + location.coordinate.lon + '&markers=color:black%7Csize:small%7C' + location.coordinate.lat + ',' + location.coordinate.lon + '&style=element:geometry.stroke|visibility:off&style=feature:landscape|element:geometry|saturation:-100&style=feature:water|saturation:-100|invert_lightness:true&zoom=11&size=200x100&key=' + googleMapsApi + "'/>" +
        '</div>' +
        "<div class='media-body'><b>" + location.illw.replace(re, function myFunction(x) {
            return '<mark>' + x + '</mark>'
        }) + '</b> ' + location.name.replace(re, function myFunction(x) {
            return '<mark>' + x + '</mark>'
        }) +
        '<br><small>' + location.country.replace(re, function myFunction(x) {
            return '<mark>' + x + '</mark>'
        }) + '</small> ' +
        '<br><small>' + location.continent.replace(re, function myFunction(x) {
            return '<mark>' + x + '</mark>'
        }) + '</small>' +
        '<br><small>' + location.dxcc.replace(re, function myFunction(x) {
            return '<mark>' + x + '</mark>'
        }) + '</small> ' +
        '</div>' +
        '</div>'

    return markup
}

function formatLocationSelection(location) {
    if (location.id === '') { // adjust for custom placeholder values
        return location.name
    }
    return '<div><b>' + location.illw + '</b> ' + location.name + '</div>'
}

function formatLogTable() {
    // Change the selector if needed
    var $table = $('table#log'),
        $bodyCells = $table.find('tbody tr:first').children(),
        colWidth

    // Get the tbody columns width array
    colWidth = $bodyCells.map(function() {
        return $(this).width()
    }).get()

    // Set the width of thead columns
    $table.find('thead tr').children().each(function(i, v) {
        $(v).width(colWidth[i])
    })
}

function formatFirebaseTable() {
    // Change the selector if needed
    var $table = $('table#firebase'),
        $bodyCells = $table.find('tbody tr:first').children(),
        colWidth

    // Get the tbody columns width array
    colWidth = $bodyCells.map(function() {
        return $(this).width()
    }).get()

    // Set the width of thead columns
    $table.find('thead tr').children().each(function(i, v) {
        $(v).width(colWidth[i])
    })
}

$(document).ready(function() {
  $('#alert').hide()
  $('#alert').fadeTo(0, 0)
  $('.location-selection-container').each(function(i, v) {
      $(v).hide()
  })

  // Initialize our JSONRPCClient
  JSONRPCClient = new $.JsonRpcClient({
      ajaxUrl: getDefaultPostURL(),
      socketUrl: getDefaultWebSocketURL(), // get a websocket for the localhost
      onmessage: onWebSocketMessage,
      onopen: onWebSocketOpen,
      onclose: onWebSocketClose,
      onerror: onWebSocketError
  })

  $('.location-search').select2({
    placeholder: {
        id: '', // the value of the option
        name: 'Type to select'
    },
    ajax: {
        delay: 250,
        transport: function(params, success, failure) {
            var $this = $(this)
            JSONRPCClient.call('search', params.data.term,
                function(result) {
                    success(result, params)
                },
                function(error) {
                    addError(error)
                })
        },
        processResults: function(data, params) {
            // parse the results into the format expected by Select2
            // since we are using custom formatting functions we do not need to
            // alter the remote JSON data, except to indicate that infinite
            // scrolling can be used

            locationTerms = params.term

            var data = $.map(data, function(obj) {
                obj.id = obj.illw
                return obj
            })

            return {
                results: data
            }
        },
        cache: true
    },
    minimumInputLength: 2,
    escapeMarkup: function(markup) {
        return markup
    }, // let our custom formatter work
    templateResult: formatLocation,
    templateSelection: formatLocationSelection
  })

  $('#illw-search .location-search').on('select2:select', function(evt) {
      selectLocation(evt.params.data, $('#illw-search'), true)
  })

  $('#locatorField').on('keyup', function(evt) {
      if (!['ArrowUp', 'ArrowDown', 'ArrowLeft', 'ArrowRight', 'Home', 'End', 'Meta', 'Alt', 'Shift', 'Control'].includes(evt.key)) {
          var value = evt.target.value
          console.log(evt)
          if (value.length > 0) {
              var c = {
                      lat: 0,
                      lon: 0
                  }
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
                  evt.target.value = filteredValue
              }

              // try converting to decimal coordinates
              try {
                  c = HamGridSquare.toLatLon(value, c)
                  $('#locatorField').parent().parent().removeClass('has-error')
                  $('#locatorField').parent().parent().addClass('has-success')
                  $('#locatorHelpBlock').html('the locator code is valid')

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
                  }, function(results, status) {
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
                                  'illw': '',
                                  'name': name,
                                  'country': country,
                                  'dxcc': dxcc,
                                  'continent': '',
                                  'coordinate': c
                              }
                              selectLocation(newLocation, $('#locator-search'), (evt.key == 'Enter'))
                          } else {
                              // empty results from google
                              $('#locatorField').parent().parent().addClass('has-warning')
                              $('#locatorField').parent().parent().removeClass('has-success')
                              $('#locatorField').parent().parent().removeClass('has-error')
                              $('#locatorHelpBlock').html('could not find a name for the locator')
                              var newLocation = {
                                  'illw': '',
                                  'name': name,
                                  'country': country,
                                  'dxcc': dxcc,
                                  'continent': '',
                                  'coordinate': c
                              }
                              selectLocation(newLocation, $('#locator-search'), (evt.key == 'Enter'))
                          }
                      } else {
                          // geocoding failed getting results from google, lets look for oceans
                          $.ajax({
                              type: 'GET',
                              url: 'http://api.geonames.org/extendedFindNearby?lat=' + c.lat + '&lng=' + c.lon + '&username=olekristensen',
                              cache: true,
                              dataType: 'xml',
                              success: function(xml) {
                                  console.log(xml)
                                  $(xml).find('geonames').each(function() {
                                      $(this).find('ocean').each(function() {
                                          var name = $(this).find('name').text()
                                          var newLocation = {
                                              'illw': '',
                                              'name': value + ', ' + name,
                                              'country': '',
                                              'dxcc': '',
                                              'continent': '',
                                              'coordinate': c
                                          }
                                          selectLocation(newLocation, $('#locator-search'), (evt.key == 'Enter'))
                                      })
                                  })
                              },
                              error: function(e) {
                                  $('#locatorField').parent().parent().addClass('has-warning')
                                  $('#locatorField').parent().parent().removeClass('has-success')
                                  $('#locatorField').parent().parent().removeClass('has-error')
                                  $('#locatorHelpBlock').html('could not find a name for the locator')
                                  var newLocation = {
                                      'illw': '',
                                      'name': value,
                                      'country': '',
                                      'dxcc': '',
                                      'continent': '',
                                      'coordinate': c
                                  }
                                  selectLocation(newLocation, $('#locator-search'), (evt.key == 'Enter'))
                              }
                          })
                      }
                  })
              } catch (e) {
                  // grid locator failed
                  $('#locatorField').parent().parent().addClass('has-error')
                  $('#locatorField').parent().parent().removeClass('has-success')
                  $('#locatorField').parent().parent().removeClass('has-warning')
                  $('#locatorHelpBlock').html(e.split(':')[1])
                  hideSelection($('#locator-search'))
              }
          } else {
              // field is empty
              $('#locatorField').parent().parent().removeClass('has-error')
              $('#locatorField').parent().parent().removeClass('has-success')
              $('#locatorField').parent().parent().removeClass('has-warning')
              hideSelection($('#locator-search'))
              $('#locatorHelpBlock').html('')
          }
      }
  })
  refreshLog()
  formatLogTable()
  initializeButtons()
  initAutocomplete()
});

document.addEventListener('DOMContentLoaded', function(event) {
    if (window.DeviceOrientationEvent) {
        //  successAlert("<strong>Success</strong> DeviceOrientationEvent API is supported on this device.");
        window.addEventListener('deviceorientation', function(eventData) {
            // gamma: Tilting the device from left to right. Tilting the device to the right will result in a positive value.
            // gamma: Het kantelen van links naar rechts in graden. Naar rechts kantelen zal een positieve waarde geven.
            var tiltLR = eventData.gamma

            // beta: Tilting the device from the front to the back. Tilting the device to the front will result in a positive value.
            // beta: Het kantelen van voor naar achteren in graden. Naar voren kantelen zal een positieve waarde geven.
            var tiltFB = eventData.beta

            // alpha: The direction the compass of the device aims to in degrees.
            // alpha: De richting waarin de kompas van het apparaat heen wijst in graden.
            var dir = eventData.alpha

            // Call the function to use the data on the page.
            // Roep de functie op om de data op de pagina te gebruiken.
            if (eventData.absolute == 1) {
                deviceOrientationHandler(tiltLR, tiltFB, dir)
            } else {
                if (initialOffset === null && eventData.absolute !== true &&
                    +eventData.webkitCompassAccuracy > 0 && +eventData.webkitCompassAccuracy < 50) {
                    initialOffset = eventData.webkitCompassHeading || 0
                }
                var alpha = eventData.alpha - initialOffset
                if (alpha < 0) {
                    alpha += 360
                }
                deviceOrientationHandler(tiltLR, tiltFB, alpha)
            }
        }, false)
    } else {
        // successAlert("<strong>Sorry</strong> DeviceOrientationEvent API is not supported on this device.");
    };
})

function deviceOrientationHandler(tiltLR, tiltFB, dir) {
    // document.getElementById("tiltLR").innerHTML = Math.ceil(tiltLR);
    // document.getElementById("tiltFB").innerHTML = Math.ceil(tiltFB);
    // document.getElementById("direction").innerHTML = Math.ceil(dir);

    // Rotate the disc of the compass.
    // Laat de kompas schijf draaien.
    var compassDisc = document.getElementById('compassDiscImg')
    compassDisc.style.webkitTransform = 'rotate(' + dir + 'deg)'
    compassDisc.style.MozTransform = 'rotate(' + dir + 'deg)'
    compassDisc.style.transform = 'rotate(' + dir + 'deg)'
}

var placeSearch, autocomplete

function initAutocomplete() {
    // Create the autocomplete object, restricting the search to geographical
    // location types.
    if ($('#autocomplete').length) {
        autocomplete = new google.maps.places.Autocomplete(
            /** @type {!HTMLInputElement} */
            (document.getElementById('autocomplete')), {
                types: ['geocode']
            })

        // When the user selects an address from the dropdown, populate the address
        // fields in the form.
        autocomplete.addListener('place_changed', function() {
            var place = autocomplete.getPlace()

            if (!place.place_id) {
                return
            }
            var country = ''
            var dxcc = ''
            var name = ''
            for (var i = 0; i < place.address_components.length; i++) {
                var addressType = place.address_components[i].types[0]
                if (addressType == 'country') {
                    dxcc = place.address_components[i].short_name
                    country = place.address_components[i].long_name
                } else {
                    if (name == '') {
                        name = place.address_components[i].long_name
                    } else {
                        name = name + ', ' + place.address_components[i].long_name
                    }
                }
            }

            var newLocation = {
                'illw': '',
                'name': name,
                'country': country,
                'dxcc': '',
                'continent': '',
                'coordinate': {
                    'lat': place.geometry.location.lat(),
                    'lon': place.geometry.location.lng()
                }
            }

            selectLocation(newLocation, $('#google-search'), true)
        })
    }
}
