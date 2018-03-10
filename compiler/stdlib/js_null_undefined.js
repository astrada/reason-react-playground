'use strict';
define(["exports"],
  function(exports){
    'use strict';
    function bind(x, f) {
      if (x == null) {
        return x;
      } else {
        return f(x);
      }
    }
    
    function iter(x, f) {
      if (x == null) {
        return /* () */0;
      } else {
        return f(x);
      }
    }
    
    function fromOption(x) {
      if (x) {
        return x[0];
      } else {
        return undefined;
      }
    }
    
    var from_opt = fromOption;
    
    exports.bind = bind;
    exports.iter = iter;
    exports.fromOption = fromOption;
    exports.from_opt = from_opt;
    
  })
/* No side effect */
