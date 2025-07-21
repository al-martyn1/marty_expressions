@set UINT8=--underlaying-type=std::uint8_t
@set UINT16=--underlaying-type=std::uint16_t
@set UINT32=--underlaying-type=std::uint32_t
@set INT=--underlaying-type=int

@set HEX2=--hex-width=2
@set HEX4=--hex-width=4
@set HEX6=--hex-width=6
@set HEX8=--hex-width=8

@rem set EXTRA=--enum-flags=extra
@set FLAGENUM_EXTRA=--enum-flags=serialize-set,deserialize-set

@set FLAGS=--enum-flags=flags
@set DECL=--enum-flags=type-decl
@set CLS=--enum-flags=enum-class
@set DECLCLS=--enum-flags=type-decl,enum-class
@set FMTHEX=--enum-flags=fmt-hex
@set SERIALIZE=--enum-flags=serialize,deserialize,lowercase

@set TPL_OVERRIDE=--override-template-parameter=EnumNameFormat:$(ENAMNAME)
@set GEN_OPTS=--enum-values-style=CamelStyle

@set VALUES_CAMEL=--enum-values-style=CamelStyle
@set SERIALIZE_CAMEL=--serialize-style=CamelStyle
@set VALUES_PASCAL=--enum-values-style=PascalStyle
@set SERIALIZE_PASCAL=--serialize-style=PascalStyle
@set SERIALIZE_HYPHEN=--serialize-style=HyphenStyle

@set TR=--tr-lang=en-US,ru-RU --tr-template-language=en-US --tr-template-output=./tr/en-US.json --translation=./enum-translations.json
@set MD=--md-output-path=../md_/_enums

@set SNIPPETOPTIONS_GEN_FLAGS=--enum-flags=0 --enum-flags=type-decl,serialize,deserialize,lowercase,enum-class,fmt-hex %VALUES_CAMEL% %SERIALIZE_PASCAL% --enum-serialize-style=All

@set FLAGS=--enum-flags=flags

@rem call %~dp0\tr.bat
umba-tr --help > umba-tr-help.txt
umba-tr --lang-tag-format=langTag --overwrite --scan=./tr --include-files=*.json enum-translations.json >umba-tr.log 2>&1

@set INPUT=^
%UINT32% %HEX2% -E=BracketKind              -F=@BracketKind.txt                    ^
%UINT32% %HEX2% -E=Error                    -F=@Error.txt                          ^
%UINT32% %HEX2% -E=ParserState              -F=@ParserState.txt                    ^
%UINT32% %HEX2% -E=ItemType                 -F=@ItemType.txt                       ^
%UINT32% %HEX2% -E=TokenContext             -F=@TokenContext.txt                   ^
%FLAGS% %FLAGENUM_EXTRA%                                                           ^
%UINT32% %HEX2% -E=OperatorFeatures         -F=@OperatorFeatures.txt               ^
%UINT32% %HEX2% -E=OperatorRestrictions     -F=@OperatorRestrictions.txt           ^
%UINT32% %HEX2% -E=OperatorAffixation       -F=@OperatorAffixation.txt             ^
%UINT32% %HEX2% -E=OperatorAssociativity    -F=@OperatorAssociativity.txt          ^
%UINT32% %HEX2% -E=OperatorArity            -F=@OperatorArity.txt


umba-enum-gen %TR% %MD% %GEN_OPTS% %HEX2% %TPL_OVERRIDE% %SNIPPETOPTIONS_GEN_FLAGS% ^
    %INPUT%                                                                         ^
..\enums.h

@rem %TR% %MD%
umba-enum-gen %GEN_OPTS% %HEX2% %TPL_OVERRIDE% %SNIPPETOPTIONS_GEN_FLAGS% ^
    --template=description-map                                                      ^
    %INPUT%                                                                         ^
..\enum_descriptions.h
