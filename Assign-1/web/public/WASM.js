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

var Search = {
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

  findPath: function(tableIn, SearchFunc) {
    let table = [];
    let posA = [];
    let action = [];

    let tableLong = [];
    for (let i = 0; i < 8; i++) {
      table[i] = [];
      for (let j = 0; j < 8; j++) {
        table[i][j] = tableIn[i][j] == "" ? " " : tableIn[i][j];
      }
      tableLong[i] = tableIn[i].join("");
    }

    tableLong = tableLong.join("");

    for (let a = 0; a < 3; a++) {
      let allies = Search.typeNum2Char(a);

      let isRun = false;
      for (let i = 0; i < 8 && !isRun; i++) {
        for (let j = 0; j < 8 && !isRun; j++) {
          if (table[i][j] == allies) {
            posA[a] = [];
            posA[a][0] = i;
            posA[a][1] = j;
            isRun = true;
          }
        }
      }
    }

    let result = Module.ccall(
      SearchFunc, // name of C function
      "string", // return type
      ["string"], // argument types
      [tableLong] // arguments
    );

    for (let i = 0, strLen = result.length; i < strLen; i++) {
      let doStore = result.charCodeAt(i);
      let out = {
        action: doStore & 1,
        allies: (doStore >> 2) & 3,
        direction: (doStore >> 4) & 3
      };
      action[i] = out;
    }

    return { table, posA, action, index: 0 };
  },

  next: function(index, action, table, posA) {
    if (index >= action.length) return null;
    let out = action[index];

    if (out.action == 0) {
      table[posA[out.allies][0]][posA[out.allies][1]] = " ";
      posA[out.allies] = Search.calcDirection(out.direction, posA[out.allies]);
      table[posA[out.allies][0]][posA[out.allies][1]] = Search.typeNum2Char(
        out.allies
      );
    } else {
      let K = Search.calcDirection(out.direction, posA[out.allies]);
      table[K[0]][K[1]] = " ";
    }

    index++;
    return { index, action, table, posA };
  },

  back: function(index, action, table, posA) {
    if (index <= 0) return null;
    index--;

    let out = action[index];

    if (out.action == 0) {
      table[posA[out.allies][0]][posA[out.allies][1]] = " ";
      posA[out.allies] = Search.calcDirection(
        out.direction,
        posA[out.allies],
        true
      );

      table[posA[out.allies][0]][posA[out.allies][1]] = Search.typeNum2Char(
        out.allies
      );
    } else {
      let K = Search.calcDirection(out.direction, posA[out.allies]);
      table[K[0]][K[1]] = Search.typeNum2Char(
        (out.allies + 1) % 3
      ).toLocaleLowerCase();
    }

    return { index, action, table, posA };
  }
};

var BFS = {
  table: [],
  action: [],
  index: 0,
  posA: [],

  findPath: function(tableIn) {
    let res = Search.findPath(tableIn, "findPathBFS");
    BFS = { ...BFS, ...res };
  },

  next: function() {
    if (BFS.index >= BFS.action.length) return null;
    let res = Search.next(BFS.index, BFS.action, BFS.table, BFS.posA);
    BFS = { ...BFS, ...res };
    return BFS.table;
  },

  back: function() {
    if (BFS.index <= 0) return null;
    let res = Search.back(BFS.index, BFS.action, BFS.table, BFS.posA);
    BFS = { ...BFS, ...res };
    return BFS.table;
  }
};

var DFS = {
  table: [],
  action: [],
  index: 0,
  posA: [],

  findPath: function(tableIn) {
    let res = Search.findPath(tableIn, "findPathDFS");
    DFS = { ...DFS, ...res };
  },

  next: function() {
    if (DFS.index >= DFS.action.length) return null;
    let res = Search.next(DFS.index, DFS.action, DFS.table, DFS.posA);
    DFS = { ...DFS, ...res };
    return DFS.table;
  },

  back: function() {
    if (DFS.index <= 0) return null;
    let res = Search.back(DFS.index, DFS.action, DFS.table, DFS.posA);
    DFS = { ...DFS, ...res };
    return DFS.table;
  }
};
