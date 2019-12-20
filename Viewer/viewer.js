
// *********************************************************************
// Global variables
// *********************************************************************

// Viewer state.
var gamePaused = true;
var gamePreview = false; // If true, render will be called for the next tick even if game is paused, and then will be set to false.
var gameAnim = true;
var gameDirection = 1;
var actRound = 0; // Current round index.


// Data.
var raw_data_str; // String for storing the raw data.
var dataLoaded = false; // Set to true when raw_data_str is ready to be parsed.
var data = { } // Object for storing all the game data.


// Animation.
var speed = 10; // Ticks per second.
var FRAMES_PER_ROUND = 2;
var frames = 0; // Incremented each tick, when it reaches FRAMES_PER_ROUND, actRound is updated (acording to gameDirection).


// Visuals.
var unitSize = 0.6; // 1 = same size as tile.
var unitLineWidth = 2;
var grid_color = "#000000";


var cell_colors = {
  '0': "#F8A858",
  '1': "#70FF70",
  '2': "#60A8FF",
  '3': "#C0A0FF",
  'C': "#FFFFFF",
  'T': "#FFD700",
  'G': "#505050",
  'A': "#000000",
  'R': "#000000",
  'd': "#101010",
  'c': "#606060",
  'b': "#B0B0B0",
  'a': "#F0F0F0",
  'O': "#A0FFFF"
}


var player_colors = {
  0: "#B06020",
  1: "#008000",
  2: "#0050BB",
  3: "#8000A0"
}


// *********************************************************************
// Utility functions
// *********************************************************************

function getURLParameter (name) {
  // http://stackoverflow.com/questions/1403888/get-url-parameter-with-jquery
  var a = (RegExp(name + '=' + '(.+?)(&|$)').exec(location.search)||[,null])[1]
  if (a != null) return decodeURI(a);
  return null;
}


// Callback has a single parameter with the file contents.
function loadFile (file, callback) {
  var xmlhttp;

  if (file == null || file == "") {
    alert("You must specify a file to load.");
    return;
  }

  if (window.XMLHttpRequest) xmlhttp = new XMLHttpRequest(); // Code for IE7+, Firefox, Chrome, Opera, Safari.
  else xmlhttp = new ActiveXObject("Microsoft.XMLHTTP"); // Code for IE6, IE5.

  // http://www.w3schools.com/ajax/ajax_xmlhttprequest_onreadystatechange.asp
  xmlhttp.onreadystatechange = function() {
    // Note: We cannot check xmlhttp.status != 200 for errors because status is not set when loading local files.
    if (xmlhttp.readyState == 4) callback(xmlhttp.responseText);
  }

  xmlhttp.open("GET", file, false);
  xmlhttp.send();
}


function int (s) {
  return parseInt(s);
}


function double (s) {
  return parseFloat(s);
}


function parse_assert (read_value, expected_value) {
  var correct = (read_value == expected_value);
  if (!correct) alert("Error parsing file, expected token: " + expected_value + ", read token: " + read_value + ".");
  return correct;
}


// *********************************************************************
// Initialization functions
// *********************************************************************

function parseData (raw_data_str) {
  if (raw_data_str == "") {
    alert("Could not load game file.");
    return false;
  }

  // Convert text to tokens.
  var st = raw_data_str + "";
  var t = st.replace('\n', ' ').split(/\s+/);
  var p = 0;

  data.secgame = (t[p++] == "SecGame");

  parse_assert(t[p++], "Seed");
  data.seed = int(t[p++]);

  // Game and version.
  if (t[p++] != "Moria") {
    alert("Are you sure this is a Moria game file?");
    document.getElementById('file').value = "";
    document.getElementById('inputdiv').style.display = "";
    document.getElementById('loadingdiv').style.display = "none";
    return false;
  }

  data.version = t[p++];
  if (data.version != "1.1") alert("Unsupported game version! Trying to load it anyway.");

  parse_assert(t[p++], "nb_players");
  data.nb_players = int(t[p++]);

  parse_assert(t[p++], "nb_rounds");
  data.nb_rounds = int(t[p++]);

  parse_assert(t[p++], "nb_dwarves");
  data.nb_dwarves = int(t[p++]);

  parse_assert(t[p++], "nb_wizards");
  data.nb_wizards = int(t[p++]);

  parse_assert(t[p++], "max_nb_orcs");
  data.max_nb_orcs = int(t[p++]);

  parse_assert(t[p++], "nb_trolls");
  data.nb_trolls = int(t[p++]);

  parse_assert(t[p++], "treasure_value");
  data.treasure_value = int(t[p++]);

  parse_assert(t[p++], "rock_resistance");
  data.rock_resistance = int(t[p++]);

  parse_assert(t[p++], "rows");
  data.rows = int(t[p++]);

  parse_assert(t[p++], "cols");
  data.cols = int(t[p++]);

  data.nb_units = data.nb_players * (data.nb_dwarves + data.nb_wizards)
                  + data.max_nb_orcs + data.nb_trolls + 1;

  parse_assert(t[p++], "names");
  data.names = new Array();
  for (var i = 0; i < data.nb_players; ++i) data.names[i] = t[p++];

  data.rounds = new Array();
  for (var round = 0; round <= data.nb_rounds; ++round) {
    // Maze.
    data.rounds[round] = new Object();
    data.rounds[round].rows = new Array();
    for (var i = 0; i < data.rows; ++i) data.rounds[round].rows[i] = t[p++];

    parse_assert(t[p++], "round");
    if (int(t[p++]) != round) alert("Wrong round number!");

    // Num cells.
    parse_assert(t[p++], "nb_cells");
    data.rounds[round].nb_cells = new Array();
    for (var i = 0; i < data.nb_players; ++i) data.rounds[round].nb_cells[i] = int(t[p++]);

    // Treasures.
    parse_assert(t[p++], "nb_treasures");
    data.rounds[round].nb_treasures = new Array();
    for (var i = 0; i < data.nb_players; ++i) data.rounds[round].nb_treasures[i] = int(t[p++]);

    data.rounds[round].points = new Array();
    for (var i = 0; i < data.nb_players; ++i)
      data.rounds[round].points[i] = data.rounds[round].nb_cells[i]
                                     + data.treasure_value*data.rounds[round].nb_treasures[i];

    // Status.
    parse_assert(t[p++], "status");
    data.rounds[round].cpu = new Array();
    for (var i = 0; i < data.nb_players; ++i) {
      var cpu = int(double(t[p++])*100);
      data.rounds[round].cpu[i] = (cpu == -100) ? "out" : cpu + "%";
    }

    // Units.
    data.rounds[round].units = [ ];
    data.rounds[round].alive_dwarves = { 0:0, 1:0, 2:0, 3:0 };
    data.rounds[round].alive_wizards = { 0:0, 1:0, 2:0, 3:0 };
    for (var i = 0; i < data.nb_units; ++i) {
      var unit = {
        type:   t[p++],
        player: int(t[p++]),
        i:      int(t[p++]),
        j:      int(t[p++]),
        health: int(t[p++]),
        move:   '' // Will be set when reading movements.
      };

      data.rounds[round].units.push(unit);
      if (unit.type == 'd') ++data.rounds[round].alive_dwarves[unit.player];
      else if (unit.type == 'w') ++data.rounds[round].alive_wizards[unit.player];
    }

    if (round != data.nb_rounds) {
      // Movements.
      parse_assert(t[p++], "movements");
      var code = int(t[p++]);
      while (code != -1) {
        data.rounds[round].units[code].move = t[p++];
        code = int(t[p++]);
      }
    }
  }
  return true;
}


// Initializing the game.
function initGame (raw_data) {
  document.getElementById("loadingdiv").style.display = "";

  // TODO: Next two calls could run concurrently.
  if (parseData(raw_data) === false) return;
  preloadImages();

  gamePaused = false;
  gamePreview = true;

  // Canvas element.
  canvas = document.getElementById('myCanvas');
  context = canvas.getContext("2d");

  // Prepare the slider.
  $("#slider").slider({
    slide: function(event, ui) {
      var value = $("#slider").slider( "option", "value" );
      actRound = value;
      frames = 0;
      gamePaused = true;
      gamePreview = true;
    }
  });
  $("#slider").width(600);
  $("#slider").slider("option", {min: 0, max: data.nb_rounds});

  // Set the listerners for interaction.
  document.addEventListener('mousewheel', onDocumentMouseWheel, false);
  document.addEventListener('keydown', onDocumentKeyDown, false);
  document.addEventListener('keyup', onDocumentKeyUp, false);

  window.addEventListener('resize', onWindowResize, false);
  onWindowResize();

  document.getElementById("loadingdiv").style.display = "none";
  document.getElementById("gamediv").style.display = "";

  mainloop();
}


function preloadImages () {
  data.img = new Array();
}


// *********************************************************************
// Main loop functions
// *********************************************************************

function updateGame () {
  $("#slider").slider("option", "value", actRound);
}


function writeGameState () {
  // Write round.
  $("#round").html("Round: " + actRound);

  // Update scoreboard.
  var scoreboard = "";
  for (var i = 0; i < data.nb_players; ++i) {
    scoreboard += "<span class='score'>"
      + "<div style='display:inline-block; margin-top: 5px; width:20px; height:20px; background-color:" + cell_colors[i] + "'></div>"
      + "<div style='display:inline-block; vertical-align: middle; margin-bottom: 7px; margin-left:8px;'>" + data.names[i] + "</div>"
      + "<br/>"
      + "<div style='margin-left: 10px;'>"
      + "<div style='padding:2px;'>Cells:     " + data.rounds[actRound].nb_cells[i] + "</div>"
      + "<div style='padding:2px;'>Treasures: " + data.rounds[actRound].nb_treasures[i] + "</div>"
      + "<div style='padding:2px;'>Dwarves:   " + data.rounds[actRound].alive_dwarves[i] + "</div>"
      + "<div style='padding:2px;'>Wizards:   " + data.rounds[actRound].alive_wizards[i] + "</div>"
      + (data.secgame ? "<div style='padding:2px;'>CPU: " + data.rounds[actRound].cpu[i] + "</div>" : "")
      + "</div>"
      + "</span><br/><br/>";
  }
  $("#scores").html(scoreboard);

  var order = [0, 1, 2, 3];
  for (var i = 0; i < 3; ++i) {
    for (var j = i + 1; j < 4; ++j) {
      if (data.rounds[actRound].points[order[i]] < data.rounds[actRound].points[order[j]]) {
        var k = order[i];
        order[i] = order[j];
        order[j] = k;
      }
    }
  }

  var totalboard = "";
  for (var i = 0; i < data.nb_players; ++i) {
    totalboard += "<span class='total'>"
      + "<div style='display:inline-block; margin-top: 5px; width:20px; height:20px; background-color:" + cell_colors[order[i]] + "'></div>"
      + "<div style='display:inline-block; vertical-align: middle; margin-bottom: 7px; margin-left:8px;'>"
      + data.rounds[actRound].points[order[i]] + "</div>"
      + "</span><br/><br/>";
  }
  $("#totals").html(totalboard);
}


function drawGame () {
  // Boundary check.
  if (actRound < 0) actRound = 0;
  if (actRound >= data.nb_rounds) actRound = data.nb_rounds;

  // Outter Rectangle.
  context.fillStyle = grid_color;
  context.fillRect(0, 0, tileSize*data.cols, tileSize*data.rows);

  // Draw maze.
  var meitat = tileSize/2;
  var quart = tileSize/4;
  var amplada = tileSize - 0.5;
  var rows = data.rounds[actRound].rows;
  for (var i = 0; i < data.rows; ++i) {
    var row = rows[i];
    for (var j = 0; j < data.cols; ++j) {
      var cell = row[j];
      var ii = i*tileSize;
      var jj = j*tileSize;
      if (cell == 'A') {
        var center_i = ii + meitat;
        var center_j = jj + meitat;
        var my_gradient = context.createRadialGradient(center_j, center_i, 0, center_j, center_i, meitat + 5);
        my_gradient.addColorStop(0, "red");
        my_gradient.addColorStop(1, "black");
        context.fillStyle = my_gradient;
        context.fillRect(jj, ii, tileSize, tileSize);
      }
      else if (cell == 'G') {
        context.strokeStyle = "#000000";
        context.fillStyle = "#000000";
        context.fillRect(jj, ii, tileSize, tileSize);
        var my_gradient = context.createLinearGradient(jj, ii + tileSize, jj + tileSize, ii);
        my_gradient.addColorStop(0, cell_colors[cell]);
        my_gradient.addColorStop(1, "#D0D0D0");
        context.fillStyle = my_gradient;
        context.fillRect(jj, ii, tileSize, tileSize);
        context.fillStyle = "#000000";
        context.strokeRect(jj + 0.5, ii + 0.5, tileSize - 1.5, tileSize - 1.5);
      }
      else if (cell >= 'a' && cell <= 'd') {
        var my_gradient = context.createLinearGradient(jj, ii, jj + tileSize, ii + tileSize);
        my_gradient.addColorStop(0, cell_colors[cell]);
        my_gradient.addColorStop(1, "#905018");
        context.fillStyle = my_gradient;
        context.fillRect(jj, ii, tileSize, tileSize);
      }
      else if (cell == 'T') {
        context.fillStyle = cell_colors['C'];
        context.fillRect(jj, ii, amplada, amplada);
        context.fillStyle = cell_colors[cell];
        var size = unitSize * tileSize * 1.2;
        var offset = (tileSize - size) / 2;
        context.beginPath();
        context.arc(jj + size/2 + offset, ii + size/2 + offset, size/2, 0, Math.PI*2, false);
        context.fill();
        context.stroke();
      }
      else {
        context.fillStyle = cell_colors[cell];
        context.fillRect(jj, ii, amplada, amplada);
      }
    }
  }

  // Draw units.
  context.lineWidth = unitLineWidth;
  var units = data.rounds[actRound].units;
  for (var un in units) {
    var u = units[un];
    if (u.player == -1) {
      context.strokeStyle = "#000000";
      context.fillStyle = "#FF0000";
    }
    else {
      context.strokeStyle = player_colors[u.player];
      context.fillStyle = player_colors[u.player];
    }
    var i = u.i;
    var j = u.j;

    if (gameAnim) {
      if (frames >= FRAMES_PER_ROUND/2) {
        if (u.move == 'b') i += 0.5;
        else if (u.move == 'w') { i += 0.5; j += 0.5; }
        else if (u.move == 'r') j += 0.5;
        else if (u.move == 'x') { i -= 0.5; j += 0.5; }
        else if (u.move == 't') i -= 0.5;
        else if (u.move == 'y') { i -= 0.5; j -= 0.5; }
        else if (u.move == 'l') j -= 0.5;
        else if (u.move == 'z') { i += 0.5; j -= 0.5; }
      }
    }

    if (u.type == 'd') drawDwarf(i, j);
    else if (u.type == 'w') drawWizard(i, j);
    else if (u.type == 'o') drawOrc(i, j);
    else if (u.type == 't') drawTroll(i, j);
    else if (u.type == 'b') drawBalrog(i, j, "#FF0000");
  }

  if (gameAnim && frames >= FRAMES_PER_ROUND/2) {
    for (var i = 0; i < data.rows; ++i) {
      var row = rows[i];
      for (var j = 0; j < data.cols; ++j) {
        if (row[j] == 'S') drawStation(i, j, "#FF0000");
      }
    }
  }
}


function drawBalrog (i, j, col) {
  context.strokeStyle = "transparent";
  context.fillStyle = col;
  context.globalAlpha = 0.5;
  var jm = (j-1)*tileSize;
  var j0 = j*tileSize;
  var j1 = (j+1)*tileSize;
  var j2 = (j+2)*tileSize;
  var im = (i-1)*tileSize;
  var i0 = i*tileSize;
  var i1 = (i+1)*tileSize;
  var i2 = (i+2)*tileSize;
  context.beginPath();
  context.moveTo(j0, im);
  context.lineTo(j1, im);
  context.lineTo(j2, i0);
  context.lineTo(j2, i1);
  context.lineTo(j1, i2);
  context.lineTo(j0, i2);
  context.lineTo(jm, i1);
  context.lineTo(jm, i0);
  context.closePath();
  context.fill();
  context.stroke();
  context.beginPath();
  context.arc(j1, i1, tileSize, 0, 0.5*Math.PI);
  context.fill();
  context.stroke();
  context.beginPath();
  context.arc(j0, i1, tileSize, 0.5*Math.PI, Math.PI);
  context.fill();
  context.stroke();
  context.beginPath();
  context.arc(j0, i0, tileSize, Math.PI, 1.5*Math.PI);
  context.fill();
  context.stroke();
  context.beginPath();
  context.arc(j1, i0, tileSize, 1.5*Math.PI, 2*Math.PI);
  context.fill();
  context.stroke();
  context.globalAlpha = 1.0;
  var size = unitSize * tileSize * 1.2;
  var offset = (tileSize - size) / 2;
  context.beginPath();
  context.arc(j*tileSize + size/2 + offset, i*tileSize + size/2 + offset, size/2, 0, 2*Math.PI, false);
  context.fill();
  context.stroke();
}


function drawWizard (i, j) {
  var size = unitSize * tileSize * 0.5;
  var offset = (tileSize - size) / 2;
  context.beginPath();
  context.arc(j*tileSize + size/2 + offset, i*tileSize + size/2 + offset, size/2, 0, 2*Math.PI, false);
  context.fill();
  context.stroke();
  context.beginPath();
  context.moveTo(j*tileSize + offset - 0.4*size, i*tileSize + offset - 0.4*size);
  context.lineTo(j*tileSize + offset + 1.4*size, i*tileSize + offset + 1.4*size);
  context.stroke();
  context.beginPath();
  context.moveTo(j*tileSize + offset + 1.4*size, i*tileSize + offset - 0.4*size);
  context.lineTo(j*tileSize + offset - 0.4*size, i*tileSize + offset + 1.4*size);
  context.stroke();
}


function drawDwarf (i, j) {
  var size = unitSize * tileSize * 0.7;
  var offset = (tileSize - size) / 2;
  context.beginPath();
  context.arc(j*tileSize + size/2 + offset, i*tileSize + size/2 + offset, size/2, 0, 2*Math.PI, false);
  context.fill();
  context.stroke();
}


function drawOrc (i, j) {
  var size = unitSize * tileSize * 0.7;
  var offset = (tileSize - size) / 2;
  context.beginPath();
  context.arc(j*tileSize + size/2 + offset, i*tileSize + size/2 + offset, size/2, 0, 2*Math.PI, false);
  context.fill();
  context.stroke();
}


function drawTroll (i, j) {
  var inc = 3;
  var amplada = tileSize - inc;
  context.fillRect(j*tileSize + inc, i*tileSize + inc, amplada - inc, amplada - inc);
  context.strokeRect(j*tileSize + inc, i*tileSize + inc, amplada - inc, amplada - inc);
}


// *********************************************************************
// Button events
// *********************************************************************

function playButton () {
  gamePaused = false;
}


function pauseButton () {
  gamePaused = true;
  gamePreview = true; // To call render again.
  frames = 0;
}


function startButton () {
  gamePaused = true;
  gamePreview = true;
  frames = 0;
  actRound = 0;
}


function endButton () {
  gamePreview = true;
  frames = 0;
  actRound = data.nb_rounds;
}


function animButton () {
  gameAnim = !gameAnim;
}


function closeButton () {
  window.close();
}


// *********************************************************************
// Keyboard and Mouse events
// *********************************************************************

function onDocumentMouseWheel (event) {
}


function onDocumentKeyDown (event) {
}


function onDocumentKeyUp (event) {
  // http://www.webonweboff.com/tips/js/event_key_codes.aspx
  switch (event.keyCode) {
    case 36: // Start.
      gamePreview = true;
      actRound = 0;
      frames = 0;
      break;

    case 35: // End.
      gamePreview = true;
      actRound = data.nb_rounds;
      frames = 0;
      break;

    case 33: // PageDown.
      gamePreview = true;
      actRound -= 10;
      frames = 0;
      break;

    case 34: // PageUp.
      gamePreview = true;
      actRound += 10;
      frames = 0;
      break;

    case 38: // ArrowUp.
    case 37: // ArrowLeft.
      gamePaused= true;
      gamePreview = true;
      --actRound;
      frames = 0;
      break;

    case 40: // ArrowDown.
    case 39: // ArrowRight.
      gamePaused = true;
      gamePreview = true;
      ++actRound;
      frames = 0;
      break;

    case 32: // Space.
      if (gamePaused) playButton();
      else pauseButton();
      break;

    case 72: // "h"
      help();
      break;

    default:
      // $("#debug").html(event.keyCode);
      break;
  }
}


function onWindowResize (event) {
  // Constants.
  var header_height = 150;
  var canvas_margin = 20;

  // Set canvas size.
  var size = Math.min(document.body.offsetWidth, document.body.offsetHeight - header_height) - canvas_margin*2;

  canvas.width  = size;
  canvas.height = size;

  var max_dimension = Math.max(data.cols,data.rows);
  tileSize = size / max_dimension;

  drawGame();
}


function help () {
  // Opens a new popup with the help page.
  var win = window.open('help.html', 'name', 'height=400, width=300');
  if (window.focus) win.focus();
  return false;
}


// *********************************************************************
// This function is called periodically.
// *********************************************************************

function mainloop () {
  // Configure buttons.
  if (gamePaused) {
    $("#but_play").show();
    $("#but_pause").hide();
  }
  else {
    $("#but_play").hide();
    $("#but_pause").show();
  }

  if (actRound < 0) actRound = 0;

  if (actRound > data.nb_rounds) {
    actRound = data.nb_rounds;
    gamePaused = true;
    frames = 0;
  }

  if (!gamePaused || gamePreview) {
    updateGame();
    drawGame();
    writeGameState();

    if (gamePreview) {
      frames = 0;
      gamePreview = false;
    }
    else {
      ++frames;
      if (frames == FRAMES_PER_ROUND) {
        frames = 0;
        actRound += gameDirection;
      }
    }
  }

  // Periodically call mainloop.
  var frame_time = 1000/speed;
  setTimeout(mainloop, frame_time);
}


// *********************************************************************
// Main function, it is called when the document is ready.
// *********************************************************************

function init () {
  // Get url parameters.
  var game;
  if (getURLParameter("sub") != null) {
    var domain = window.location.protocol + "//" + window.location.host;
    if (getURLParameter("nbr") != null)
      game = domain + "/?cmd=lliuraments&sub=" + getURLParameter("sub") + "&nbr=" + getURLParameter("nbr") + "&download=partida";
    else
      game = domain + "/?cmd=partida&sub=" + getURLParameter("sub") + "&download=partida";
  }
  else game = getURLParameter("game");

  if (game == null || game == "") {
    // Ask the user for a game input.
    var inputdiv = document.getElementById('inputdiv')
    inputdiv.style.display = "";
    document.getElementById('file').addEventListener('change', function(evt) {
      //http://www.html5rocks.com/en/tutorials/file/dndfiles/
      var file = evt.target.files[0];
      var reader = new FileReader();
      reader.readAsText(file);
      reader.onloadend = function(evt) {
        if (evt.target.readyState != FileReader.DONE) alert("Error accessing file.");
        else { // DONE == 2.
          inputdiv.style.display = "none";
          document.getElementById("loadingdiv").style.display = "";
          initGame(reader.result);
        }
      };
    }, false);
  }
  else {
    document.getElementById("loadingdiv").style.display = "";
    // Load the given game.
    loadFile(game, initGame);
  }
}
