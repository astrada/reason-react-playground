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

[@bs.send] external compile : (compiler, string) => compilerResult = "";

type console;

type consoleError = string => unit;

[@bs.val] external console : console = "";

[@bs.get] external getConsoleError : console => consoleError = "error";

[@bs.set] external setConsoleError : (console, consoleError) => unit = "error";

let warningStartRegex = [%re "/^File /"];

let warningEndRegex = [%re "/^$/"];

let warningToExcludeRegex = [%re "/Warning 40/"];

let compile = code => {
  let filteredWarnings = ref([||]);
  let filterBuckleScriptWarnings = warningLine => {
    if (Js.Re.test(warningLine, warningStartRegex)) {
      filteredWarnings := [||];
    };
    Js.Array.push(warningLine, filteredWarnings^) |> ignore;
    if (Js.Re.test(warningLine, warningEndRegex)) {
      if (Js.Array.some(
            line => Js.Re.test(line, warningToExcludeRegex),
            filteredWarnings^
          )) {
        [||];
      } else {
        filteredWarnings^;
      };
    } else {
      [||];
    };
  };
  let warningArray = ref([||]);
  let outputWarning = s => {
    let warnings = filterBuckleScriptWarnings(s);
    if (Js.Array.length(warnings) > 0) {
      warningArray := Js.Array.concat(warnings, warningArray^);
    };
  };
  let standardConsoleError = getConsoleError(console);
  setConsoleError(console, outputWarning);
  let compilerResult = compile(ocaml, code);
  setConsoleError(console, standardConsoleError);
  let warnings = Js.Array.joinWith("\n", warningArray^);
  (compilerResult, warnings);
};
