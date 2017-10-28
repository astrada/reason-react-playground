type compilerResult = {
  .
  "js_code": Js.nullable(string),
  "js_error_msg": Js.nullable(string),
  "row": Js.nullable(int),
  "column": Js.nullable(int),
  "endRow": Js.nullable(int),
  "endColumn": Js.nullable(int),
  "text": Js.nullable(string),
  "_type": Js.nullable(string)
};

let compile: string => compilerResult;
