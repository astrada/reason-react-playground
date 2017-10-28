type refmtResult = {
  ocamlCode: option(string),
  reasonCode: option(string),
  errorMessage: option(string)
};

let refmtRE2ML: string => refmtResult;

let refmtML2RE: string => refmtResult;
