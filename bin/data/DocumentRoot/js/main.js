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

var JSONRPCClient; ///< The core JSONRPC WebSocket client.

var selectedLocation;

function addError(error) {
  console.log(error);
}

function onWebSocketOpen(ws) {
  console.log("on open");
  console.log(ws);
}

function onWebSocketMessage(evt) {
  console.log("on message:");
  console.log(evt.data);
}

function onWebSocketClose() {
  console.log("on close");
}

function onWebSocketError() {
  console.log("on error");
}

function addLogEntry(location) {
  var $this = $(this);
  JSONRPCClient.call('addLogEntry',
      selectedLocation,
      function(result) {
        if(result != null){
          refreshLog();
          $("#success-alert").html("<strong>" + result.destination.illw + "</strong> " + result.destination.name + " has been logged.");
          $("#success-alert").fadeTo(2000, 500, function(){
            $("#success-alert").fadeTo(2000,0);
          });
          $("#location-selection-container").slideUp(500);
          $(".location-search").val("").trigger("change");
          $(".location-search").select2("open");
        }
      },
      function(error) {
          addError(error);
      });
}

function refreshLog(){
  var $this = $(this);
  var text = $("#log").val();
  var lines = text.split(/\r|\r\n|\n/);
  var count = lines.length;
  console.log(count);
  JSONRPCClient.call('getLog',
      count-1,
      function(result) {
        console.log(result);
        if(result.length > 0){
          var logStr = [];
          $.each(result, function(idx, obj) {
            var str = obj.timestamp + " : " + obj.source.name + " >> " + obj.destination.name;
            logStr.push(str);
          });
          console.log(logStr.join("\n"));
          $('#log').val($('#log').val()+"\n"+logStr.join("\n"));
          $('#log').scrollTop($('#log')[0].scrollHeight);
        }
      },
      function(error) {
          addError(error);
      });
}

function initializeButtons() {

  $('#add-log-entry').on('click', function() {
    addLogEntry(selectedLocation);
  });

  $('#refresh-log').on('click', function() {
    refreshLog();
  });
}

function selectLocation(location){

  var markup = "<div class='media'>" +
  "<div class='pull-right'>" +
  "<img class='media-object img-rounded' src='https://maps.googleapis.com/maps/api/staticmap?maptype=terrain&center=" + location.coordinate.lat + "," + location.coordinate.lon + "&markers=color:black%7Csize:small%7C" + location.coordinate.lat + "," + location.coordinate.lon + "&style=element:geometry.stroke|visibility:off&style=feature:landscape|element:geometry|saturation:-100&style=feature:water|saturation:-100|invert_lightness:true&zoom=13&size=400x120&key=AIzaSyD7GFqO4gfF7J9ycptEaCNLLGU6i3TnaQ8'/>" +
  "</div>" +
  "<div class='media-body'><b>" + location.illw + "</b> " + location.name +
  "<br><small>" + location.country + "</small> " +
  "<br><small>" + location.continent + "</small> " +
  "<br><small>" + location.dxcc + "</small> " +
  "<br><small>" + location.coordinate.lat + "</small> " +
  "<br><small>" + location.coordinate.lon + "</small> " +
  "</div>" +
  "</div>";

  $('#location-selection').html(markup);
  $("#location-selection-container").slideDown(500);
  selectedLocation = location;
  $('#add-log-entry').focus();
}

function formatLocation (location) {
  if (location.loading) return "Fetching results from server...";

  var markup = "<div class='media'>" +
  "<div class='pull-right'>" +
  "<img class='media-object img-rounded' src='https://maps.googleapis.com/maps/api/staticmap?maptype=terrain&center=" + location.coordinate.lat + "," + location.coordinate.lon + "&markers=color:black%7Csize:small%7C" + location.coordinate.lat + "," + location.coordinate.lon + "&style=element:geometry.stroke|visibility:off&style=feature:landscape|element:geometry|saturation:-100&style=feature:water|saturation:-100|invert_lightness:true&zoom=11&size=200x100&key=AIzaSyD7GFqO4gfF7J9ycptEaCNLLGU6i3TnaQ8'/>" +
  "</div>" +
  "<div class='media-body'><b>" + location.illw + "</b> " + location.name +
  "<br><small>" + location.country + "</small> " +
  "<br><small>" + location.continent + "</small>" +
  "<br><small>" + location.dxcc + "</small> " +
  "</div>" +
  "</div>";
  return markup;

}

function formatLocationSelection (location) {
  if (location.id === '') { // adjust for custom placeholder values
      return location.name;
    }
  return "<div><b>" + location.illw + "</b> " + location.name + "</div>"
}

$(document).ready(function() {

  $("#success-alert").hide();
  $("#success-alert").fadeTo(0,0);
  $('#location-selection-container').hide();

  // Initialize our JSONRPCClient
  JSONRPCClient = new $.JsonRpcClient({
    ajaxUrl: getDefaultPostURL(),
    socketUrl: getDefaultWebSocketURL(), // get a websocket for the localhost
    onmessage: onWebSocketMessage,
    onopen: onWebSocketOpen,
    onclose: onWebSocketClose,
    onerror: onWebSocketError
  });

  $(".location-search").select2({
    placeholder: {
      id: '', // the value of the option
      name: 'Type to select'
    },
    ajax: {
        transport: function (params, success, failure) {
          var $this = $(this);
          JSONRPCClient.call('search', params.data.term,
          function(result) {
            success(result, params);
          },
          function(error) {
            addError(error);
          });
        },
        processResults: function (data, params) {
          // parse the results into the format expected by Select2
          // since we are using custom formatting functions we do not need to
          // alter the remote JSON data, except to indicate that infinite
          // scrolling can be used
          console.log(data);

          var data = $.map(data, function (obj) {
            obj.id = obj.illw;
            return obj;
          });

          return {
            results: data
          };
        },
        cache: true
      },
      minimumInputLength: 2,
      escapeMarkup: function (markup) { return markup; }, // let our custom formatter work
      templateResult: formatLocation,
      templateSelection: formatLocationSelection
  });

  $('.location-search').on('select2:select', function (evt) {
    selectLocation(evt.params.data);
  });

  initializeButtons();

});
