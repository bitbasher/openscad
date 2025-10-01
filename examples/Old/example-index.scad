/*
 Data retreival routines for documenting exmaples

 the data for this folder of examples is kept in a
 local JSON file with this format
 {
 "group": "old",
 "examples" :
    [
    {
        "number": 0,
        "name": ["place","holder"],
        "descr": "ignore this one",
        "attach":[
            ["_MYNAME_NUM_","dxf"]
        ]
    }
    ]
 }
 see function makeName() below for how this data may be used

 Written by Jeff Hayes <vulcan_at_mac_com> 2025

 To the extent possible under law, the author(s) have dedicated all
 copyright and related and neighboring rights to this software to the
 public domain worldwide. This software is distributed without any
 warranty.

 You should have received a copy of the CC0 Public Domain
 Dedication along with this software.
 If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
 */
debug_on = false;

/* [Hidden] */
gd = import( "./example-index-data.json" );

//echo( gd );

function examp_group() =
    gd.group;
    ;
function examp_number( id ) =
    gd.examples[id].number;
    ;
function examp_name( id ) =
    gd.examples[id].name;
    ;
function examp_file( id,long=true ) =
    let( exObj=examp_obj(id) )
    long ?
        makeFullFileName(exObj)
    :
        makeFileName(exObj)
    ;
function examp_attach( id ) =
    gd.examples[id].attach;
    ;
function examp_descr( id ) =
    gd.examples[id].descr;
    ;
function examp_obj( id ) =
    object([
        ["num",gd.examples[id].number],
        ["nam",gd.examples[id].name],
        ["des",gd.examples[id].descr],
        ["att",(is_undef(gd.examples[id].attach) ? [] : gd.examples[id].attach)]
        ]);
    ;

/* Numbers up to 3 digits long are formatted with leading zeros
 */
function padNumber(num) =
    num <= 0 ?
        undef
    : num <10 ?
        str( "00", num )
    : num <100 ?
        str( "0", num )
    : str( num )
    ;
if( debug_on )
    echo( padNumber(3), padNumber(30), padNumber(300));

function makeNameByID(id,delim=" ") =
    makeName(examp_obj(id),delim=delim)
    ;
/*
 The "name" element of a data object may be a string or
  a vector of words.
  String: the string is used as is
  Vector: the intention is to make a filename with the string
   "delim" between each word OR to make a title of the words
   separated by the default blank, " ", character.
 */
function makeName(exObj,delim=" ") =
    is_undef(exObj) || ! is_object(exObj)?
        str("need example object")
    : is_string(exObj.nam) ?
        makeNameString(exObj=exObj,delim=delim)
    :
        makeNameVector(exObj=exObj,delim=delim)
    ;

function makeShortName(exObj) =
    str( "example", padNumber(exObj.num) )
    ;

if( debug_on ) {
    ex1=examp_obj(7);
    echo(short=ex1);
    echo(short=makeShortName(ex1) );
    }

function makeNameString(exObj,delim) =
    str( makeShortName(exObj), delim, exObj.nam)
    ;

function makeNameVector(exObj,i=undef,delim) =
    is_undef(i) ?
        str( makeShortName(exObj),
            makeNameVector(exObj,0,delim=delim)
            )
    : i == len(exObj.nam)-1 ?
        str( delim, exObj.nam[i] ) // last word
    : str( delim, exObj.nam[i],
        makeNameVector(exObj, i+1, delim=delim)
        )
    ;

if( debug_on ) {
    testObj = object([
        ["num",1],
        ["nam", ["one","two"]],
        ["des", "example one is good"]
        ]);
    echo(mock1=testObj);
    testObj2=object(testObj, nam="fred flintstone");
    echo(mock2=testObj2);

    echo( mns=makeName( testObj2) );
    echo( mnv=makeName( testObj ) );
    echo( mnvd=makeName( testObj, "-" ) );
    }

function makeFileName(exObj,ext="scad") =
    str( makeShortName(exObj), ".", ext )
    ;
function makeFullFileName(exObj,ext="scad") =
    str( makeName(exObj,"-"), ".", ext )
    ;
if( debug_on ) {
    ex1=examp_obj(7);
    echo( fn=makeFileName(ex1) );
    echo( fne=makeFileName(ex1,"dat") );
    echo( ffn=makeFullFileName(ex1) );
    echo( ffne=makeFullFileName(ex1,"dat") );
    }

function announceExample( exNum ) =
    let( anObj=examp_obj(exNum) )
    str( "\n\t**\n\tSCAD Version ", version(),
        "\n\t",makeName(anObj),
        "\n\t",anObj.des,
        "\n\t**"
       );

if( debug_on )
    echo(ann=announceExample(7));

function isName(att,useName) =
    str( (att.x=="_MYNAME_NUM_" ? useName : att.x ), ".", att.y)
    ;
if(debug_on) {
    echo(is=isName(examp_attach(7)[0], makeShortName(examp_obj(7) ) ) );
    echo(attName=examp_attach(7),
        isName(examp_attach(7)[0], makeShortName(examp_obj(7)))
        );
    echo(attName=examp_attach(15),
        isName(examp_attach(15)[0], makeShortName(examp_obj(15)))
        );
    }

function makeAttachText(exObj,i=undef) =
    let( attVec=exObj.att )
    len(attVec) <= 0 ?
        "none"
    : is_undef(i) ?
        makeAttachText(exObj,0)
    : i == len(attVec)-1 ?
        str( "\n    ",isName(attVec[i], makeShortName(exObj)) ) // last word
    :
        str( "\n    ",isName(attVec[i], makeShortName(exObj)), makeAttachText(exObj,i+1))
    ;

if(debug_on) {
    echo( mat=makeAttachText(examp_obj( 7)) );
    echo( mat=makeAttachText(examp_obj(15)) );
    }

module printName(id) {
    str1=str("Example ",examp_number(id));
    str2=str("Title   ", makeNameByID(id)  );
    str3=str("Script File Name ", examp_file(id,false));
    str4=str("Script Long Name ", examp_file(id));
    str5=str("Description ", examp_descr(id));
    str6=str("Attachments ", makeAttachText(examp_obj(id)) );

    echo( str(
    "\n  ", str1,
    "\n  ", str2,
    "\n  ", str3,
    "\n  ", str4,
    "\n  ", str5,
    "\n  ", str6,
    "\n"
    ));
}
//printName( 3);
//printName( 7);
//printName(15);

module printNames() {
    echo( str("Examples in Group ",gd.group) );
    for(i=[1:len( gd.examples)-1])
        printName(i);
}
printNames();

/*
if( debug_on ) {
    for( ex = gd.examples )
        //echo(ex);
        echo( ex.number, ex.name, ex.descr );

    for( i = [0:len(gd.examples)-1] )
        //echo(ex);
        echo( examp_number(i), examp_name(i), examp_descr(i) );

    for( i = [0:len(gd.examples)-1] )
        echo( examp_obj( i ) );
    }
 */
