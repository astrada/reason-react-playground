type compiler;

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

[@bs.val] external ocaml : compiler = "";

[@bs.send] external compile : (compiler, string) => string = "";

[@bs.val] [@bs.scope "JSON"] external parse : string => compilerResult = "";

type console;

type consoleError = string => unit;

[@bs.val] external console : console = "";

[@bs.get] external getConsoleError : console => consoleError = "error";

[@bs.set] external setConsoleError : (console, consoleError) => unit = "error";

let compile = (code) => {
  let warningArray = ref([||]);
  let outputWarning = (s) => Js.Array.push(s, warningArray^) |> ignore;
  let standardConsoleError = getConsoleError(console);
  setConsoleError(console, outputWarning);
  let compilerResult = parse(compile(ocaml, code));
  setConsoleError(console, standardConsoleError);
  let warnings = Js.Array.joinWith("\n", warningArray^);
  (compilerResult, warnings)
};
