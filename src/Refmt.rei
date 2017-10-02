type compilerResult = Js.t {. ocamlCode : string, errorMessage : string};

let refmt: string => compilerResult;
