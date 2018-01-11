let defaultStorageKey = "reason-react-playground";

type code =
  | Null
  | Reason(string)
  | OCamlJsx(string);

let reasonLabel = "reason";

let ocamlJsxLabel = "ocaml-jsx";

let reasonQueryPrefix = "?" ++ reasonLabel ++ "=";

let ocamlJsxQueryPrefix = "?" ++ ocamlJsxLabel ++ "=";

let code2jsData =
  fun
  | Null => Js.Nullable.null
  | Reason(code) => Js.Nullable.return({"language": reasonLabel, "code": code})
  | OCamlJsx(code) =>
    Js.Nullable.return({"language": ocamlJsxLabel, "code": code});

let jsData2code = data =>
  switch (data) {
  | None => Null
  | Some(data) =>
    if (data##language == reasonLabel) {
      Reason(data##code);
    } else if (data##language == ocamlJsxLabel) {
      OCamlJsx(data##code);
    } else {
      Null;
    }
  };

[@bs.val] [@bs.scope "window"] external location : Dom.location = "";

[@bs.get]
external getLocationSearch : Dom.location => Js.nullable(string) = "search";

[@bs.val] [@bs.scope "window"] external history : Dom.history = "";

[@bs.send]
external replaceState :
  (Dom.history, Js.nullable(Js.t({..})), string, string) => unit =
  "replaceState";

let getCodeFromQueryString = () => {
  let search = Js.Nullable.to_opt(getLocationSearch(location));
  switch (search) {
  | None => Null
  | Some(s) =>
    let reasonPrefixLen = String.length(reasonQueryPrefix);
    let ocamlJsxPrefixLen = String.length(ocamlJsxQueryPrefix);
    let len = String.length(s);
    if (len > reasonPrefixLen
        && String.sub(s, 0, reasonPrefixLen) == reasonQueryPrefix) {
      let compressedCode =
        String.sub(s, reasonPrefixLen, len - reasonPrefixLen);
      let code = LzString.decompressFromEncodedURIComponent(compressedCode);
      Reason(code);
    } else if (len > String.length(ocamlJsxQueryPrefix)
               && String.sub(s, 0, ocamlJsxPrefixLen) == ocamlJsxQueryPrefix) {
      let compressedCode =
        String.sub(s, ocamlJsxPrefixLen, len - ocamlJsxPrefixLen);
      let code = LzString.decompressFromEncodedURIComponent(compressedCode);
      OCamlJsx(code);
    } else {
      Null;
    };
  };
};

let setQueryStringFromCode = code => {
  let replaceUrl = (code, queryPrefix) => {
    let compressedCode = LzString.compressToEncodedURIComponent(code);
    let url = queryPrefix ++ compressedCode;
    replaceState(history, Js.Nullable.null, "", url);
  };
  switch (code) {
  | Null => ()
  | Reason(code) => replaceUrl(code, reasonQueryPrefix)
  | OCamlJsx(code) => replaceUrl(code, ocamlJsxQueryPrefix)
  };
};

let getCodeFromStorage = () => {
  let jsDataVow = LocalForage.getItem(defaultStorageKey);
  Vow.Result.map(jsData => Vow.Result.return(jsData2code(jsData)), jsDataVow);
};

let saveCodeToStorage = code => {
  let jsData = code2jsData(code);
  let vow = LocalForage.setItem(defaultStorageKey, jsData);
  Vow.Result.sideEffect((_) => (), vow);
};

let loadPersistedState = () => {
  let codeFromQueryString = getCodeFromQueryString();
  switch (codeFromQueryString) {
  | Null => getCodeFromStorage()
  | Reason(_)
  | OCamlJsx(_) => Vow.Result.return(codeFromQueryString)
  };
};

let saveState = code =>
  switch (code) {
  | Null => ()
  | Reason(_)
  | OCamlJsx(_) =>
    setQueryStringFromCode(code);
    saveCodeToStorage(code);
  };
