var Module = {
  preRun: [],
  postRun: [],
  print: (function() {
    return function(text) {
      if (arguments.length > 1)
        text = Array.prototype.slice.call(arguments).join(" ");
      console.log(text);
    };
  })(),
  printErr: function(text) {
    if (arguments.length > 1)
      text = Array.prototype.slice.call(arguments).join(" ");
    console.error(text);
  },
  canvas: [],
  setStatus: function(text) {
    if (!Module.setStatus.last)
      Module.setStatus.last = { time: Date.now(), text: "" };
    if (text === Module.setStatus.last.text) return;
    var m = text.match(/([^(]+)\((\d+(\.\d+)?)\/(\d+)\)/);
    var now = Date.now();
    if (m && now - Module.setStatus.last.time < 30) return; // if this is a progress update, skip it if too soon
    Module.setStatus.last.time = now;
    Module.setStatus.last.text = text;
    if (m) {
      text = m[1];
      console.log("progress", parseInt(m[2]) * 100, "/", parseInt(m[4]) * 100);
    }
    console.log("status", text);
  },
  totalDependencies: 0,
  monitorRunDependencies: function(left) {
    this.totalDependencies = Math.max(this.totalDependencies, left);
    Module.setStatus(
      left
        ? "Preparing... (" +
            (this.totalDependencies - left) +
            "/" +
            this.totalDependencies +
            ")"
        : "All downloads complete."
    );
  }
};
Module.setStatus("Downloading...");
window.onerror = function() {
  Module.setStatus("Exception thrown, see JavaScript console");
  Module.setStatus = function(text) {
    if (text) Module.printErr("[post-exception status] " + text);
  };
};

var BFS = {
  table: [
    ["a", "l", "s", " ", " ", " ", " ", " "],
    ["a", "l", "a", " ", " ", " ", " ", " "],
    ["s", "s", "l", " ", " ", " ", " ", " "],
    ["a", "l", "s", " ", " ", " ", " ", " "],
    [" ", " ", " ", " ", " ", " ", " ", " "],
    [" ", " ", " ", " ", " ", " ", " ", "S"],
    [" ", " ", " ", " ", " ", " ", " ", "A"],
    [" ", " ", " ", " ", " ", " ", " ", "L"]
  ],

  bufView: [],
  bufI: 0,

  posA: [],

  typeNum2Char: function(type) {
    switch (type) {
      case 0:
        return "L";
      case 1:
        return "S";
      case 2:
        return "A";
      default:
        return " ";
    }
  },

  calcDirection: function(direction, pos, revert) {
    if (revert == true) {
      direction = (direction & (1 << 1)) | !(direction & 1);
    }
    switch (direction) {
      case 0:
        return [pos[0] - 1, pos[1]];
      case 1:
        return [pos[0] + 1, pos[1]];
      case 2:
        return [pos[0], pos[1] - 1];
      case 3:
        return [pos[0], pos[1] + 1];
    }
  },

  findPath: function(tableIn) {
    let tableLong = [];
    for (let i = 0; i < 8; i++) {
      for (let j = 0; j < 8; j++) {
        BFS.table[i][j] = tableIn[i][j];
      }
      tableLong[i] = tableIn[i].join("");
    }

    tableLong = tableLong.join("");

    for (let a = 0; a < 3; a++) {
      let allies = BFS.typeNum2Char(a);

      let isRun = false;
      for (let i = 0; i < 8 && !isRun; i++) {
        for (let j = 0; j < 8 && !isRun; j++) {
          if (BFS.table[i][j] == allies) {
            BFS.posA[a] = [];
            BFS.posA[a][0] = i;
            BFS.posA[a][1] = j;
            isRun = true;
          }
        }
      }
    }

    let result = Module.ccall(
      "findPath", // name of C function
      "string", // return type
      ["string"], // argument types
      [tableLong] // arguments
    );

    for (let i = 0, strLen = result.length; i < strLen; i++) {
      let doStore = result.charCodeAt(i);
      let out = {
        /*
        unsigned char action    : 1; /// 0 -> walk  1 -> kill
        unsigned char isSet     : 1;
        unsigned char allies    : 2; /// 0 -> L  1 -> S  2 -> A
        unsigned char direction : 2; /// 0 -> up  1 -> down  2 -> left  3 -> right
        unsigned char none      : 2;
        */
        action: doStore & 1,
        // isSet     : (doStore >> 1) & 1,
        allies: (doStore >> 2) & 3,
        direction: (doStore >> 4) & 3
      };
      BFS.bufView[i] = out;
    }
  },

  next: function() {
    if (BFS.bufI >= BFS.bufView.length) return null;

    let out = BFS.bufView[BFS.bufI];

    if (out.action == 0) {
      BFS.table[BFS.posA[out.allies][0]][BFS.posA[out.allies][1]] = " ";
      BFS.posA[out.allies] = BFS.calcDirection(
        out.direction,
        BFS.posA[out.allies]
      );
      BFS.table[BFS.posA[out.allies][0]][
        BFS.posA[out.allies][1]
      ] = BFS.typeNum2Char(out.allies);
    } else {
      let K = BFS.calcDirection(out.direction, BFS.posA[out.allies]);
      BFS.table[K[0]][K[1]] = " ";
    }

    BFS.bufI++;
    return BFS.table;
  },

  back: function() {
    if (BFS.bufI <= 0) return null;
    BFS.bufI--;

    let out = BFS.bufView[BFS.bufI];

    if (out.action == 0) {
      BFS.table[BFS.posA[out.allies][0]][BFS.posA[out.allies][1]] = " ";
      BFS.posA[out.allies] = BFS.calcDirection(
        out.direction,
        BFS.posA[out.allies],
        true
      );

      BFS.table[BFS.posA[out.allies][0]][
        BFS.posA[out.allies][1]
      ] = BFS.typeNum2Char(out.allies);
    } else {
      let K = BFS.calcDirection(out.direction, BFS.posA[out.allies]);
      BFS.table[K[0]][K[1]] = BFS.typeNum2Char(
        (out.allies + 1) % 3
      ).toLocaleLowerCase();
    }

    return BFS.table;
  }
};
