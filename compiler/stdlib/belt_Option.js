'use strict';
define(["exports", "./curry.js"],
  function(exports, Curry){
    'use strict';
    function getExn(param) {
      if (param) {
        return param[0];
      } else {
        throw new Error("getExn");
      }
    }
    
    function mapWithDefaultU(opt, $$default, f) {
      if (opt) {
        return f(opt[0]);
      } else {
        return $$default;
      }
    }
    
    function mapWithDefault(opt, $$default, f) {
      return mapWithDefaultU(opt, $$default, Curry.__1(f));
    }
    
    function mapU(opt, f) {
      if (opt) {
        return /* Some */[f(opt[0])];
      } else {
        return /* None */0;
      }
    }
    
    function map(opt, f) {
      return mapU(opt, Curry.__1(f));
    }
    
    function flatMapU(opt, f) {
      if (opt) {
        return f(opt[0]);
      } else {
        return /* None */0;
      }
    }
    
    function flatMap(opt, f) {
      return flatMapU(opt, Curry.__1(f));
    }
    
    function getWithDefault(opt, $$default) {
      if (opt) {
        return opt[0];
      } else {
        return $$default;
      }
    }
    
    function isSome(param) {
      if (param) {
        return /* true */1;
      } else {
        return /* false */0;
      }
    }
    
    function isNone(param) {
      if (param) {
        return /* false */0;
      } else {
        return /* true */1;
      }
    }
    
    function eqU(a, b, f) {
      if (a) {
        if (b) {
          return f(a[0], b[0]);
        } else {
          return /* false */0;
        }
      } else if (b) {
        return /* false */0;
      } else {
        return /* true */1;
      }
    }
    
    function eq(a, b, f) {
      return eqU(a, b, Curry.__2(f));
    }
    
    function cmpU(a, b, f) {
      if (a) {
        if (b) {
          return f(a[0], b[0]);
        } else {
          return 1;
        }
      } else if (b) {
        return -1;
      } else {
        return 0;
      }
    }
    
    function cmp(a, b, f) {
      return cmpU(a, b, Curry.__2(f));
    }
    
    exports.getExn = getExn;
    exports.mapWithDefaultU = mapWithDefaultU;
    exports.mapWithDefault = mapWithDefault;
    exports.mapU = mapU;
    exports.map = map;
    exports.flatMapU = flatMapU;
    exports.flatMap = flatMap;
    exports.getWithDefault = getWithDefault;
    exports.isSome = isSome;
    exports.isNone = isNone;
    exports.eqU = eqU;
    exports.eq = eq;
    exports.cmpU = cmpU;
    exports.cmp = cmp;
    
  })
/* No side effect */
