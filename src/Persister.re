let defaultStorageKey = "reason-react-playground";

type code =
  | Null
  | Reason(string)
  | OCaml(string);

type jsData = {. language: string, code: string};

let reasonLabel = "reason";

let ocamlLabel = "ocaml";

let reasonQueryPrefix = "?" ++ reasonLabel ++ "=";

let ocamlQueryPrefix = "?" ++ ocamlLabel ++ "=";

let code2jsData =
  fun
  | Null => Js.Nullable.null
  | Reason(code) => Js.Nullable.return({"language": reasonLabel, "code": code})
  | OCaml(code) => Js.Nullable.return({"language": ocamlLabel, "code": code});

let jsData2code = (data) =>
  switch data {
  | None => Null
  | Some(data) =>
    if (data##language == ocamlLabel) {
      OCaml(data##code)
    } else {
      Reason(data##code)
    }
  };

[@bs.val] [@bs.scope "window"] external location : Dom.location = "";

[@bs.get] external getLocationSearch : Dom.location => Js.nullable(string) = "search";

[@bs.val] [@bs.scope "window"] external history : Dom.history = "";

[@bs.send] external replaceState : (Dom.history, Js.nullable(Js.t({..})), string, string) => unit =
  "replaceState";

let getCodeFromQueryString = () => {
  let search = Js.Nullable.to_opt(getLocationSearch(location));
  switch search {
  | None => Null
  | Some(s) =>
    let reasonPrefixLen = String.length(reasonQueryPrefix);
    let ocamlPrefixLen = String.length(ocamlQueryPrefix);
    let len = String.length(s);
    if (len > reasonPrefixLen && String.sub(s, 0, reasonPrefixLen) == reasonQueryPrefix) {
      let compressedCode = String.sub(s, reasonPrefixLen, len - reasonPrefixLen);
      let code = LzString.decompressFromEncodedURIComponent(compressedCode);
      Reason(code)
    } else if (len > String.length(ocamlQueryPrefix)
               && String.sub(s, 0, ocamlPrefixLen) == ocamlQueryPrefix) {
      let compressedCode = String.sub(s, ocamlPrefixLen, len - ocamlPrefixLen);
      let code = LzString.decompressFromEncodedURIComponent(compressedCode);
      OCaml(code)
    } else {
      Null
    }
  }
};

let setQueryStringFromCode = (code) => {
  let replaceUrl = (code, queryPrefix) => {
    let compressedCode = LzString.compressToEncodedURIComponent(code);
    let url = queryPrefix ++ compressedCode;
    replaceState(history, Js.Nullable.null, "", url)
  };
  switch code {
  | Null => ()
  | Reason(code) => replaceUrl(code, reasonQueryPrefix)
  | OCaml(code) => replaceUrl(code, ocamlQueryPrefix)
  }
};

let getCodeFromStorage = () => {
  let jsDataVow = LocalForage.getItem(defaultStorageKey);
  Vow.Result.map((jsData) => Vow.Result.return(jsData2code(jsData)), jsDataVow)
};

let saveCodeToStorage = (code) => {
  let jsData = code2jsData(code);
  let vow = LocalForage.setItem(defaultStorageKey, jsData);
  Vow.Result.sideEffect((_) => (), vow)
};

let loadPersistedState = () => {
  let codeFromQueryString = getCodeFromQueryString();
  switch codeFromQueryString {
  | Null => getCodeFromStorage()
  | Reason(_)
  | OCaml(_) => Vow.Result.return(codeFromQueryString)
  }
};

let saveState = (code) =>
  switch code {
  | Null => ()
  | Reason(_)
  | OCaml(_) =>
    setQueryStringFromCode(code);
    saveCodeToStorage(code)
  };
