document.write('<script src="'+ToolPfad+'anims/'+animation+'"></script>');
document.write('<link href="config.css" rel="stylesheet" type="text/css">');

function getitall(eingangsmeldung, chatpartner, uin, pfad, allparameter) {
 
  allparameter = allparameter.split(':');
  ausgabe = eingangsmeldung;
   
  ausgabe = parser(ausgabe);
  ausgabe = tZersOutput(ausgabe,chatpartner,allparameter[0],allparameter[1],allparameter[2])
  ausgabe = linkconvert(ausgabe,allparameter[3],allparameter[4],allparameter[5],allparameter[6]);
  
  document.write(ausgabe);
  
  test = "0"; 
  if (test=="1") {
    for (index=0; index<allparameter.length; index++) {
      document.write('<br>allparameter['+index+'] : '+allparameter[index])
    }
  }

}

// video

function videoplay(message,idname,hoch,breit,popupfenster) {

einstellungspruefung_videos(Videobutton,1,'Videobutton','config.js','\"0\" -> Beide Buttons öffnen und schließen den Anzeigebereich\n\"1\" -> Beide Buttons öffnen ein neues Anzeigefenster\n\"2\" -> Unterschiedliches Verhalten der Buttons');

var tidy_message  = message.replace(/\&amp\;/g,'&');
var my_regexps    = new Array();
var my_templates  = new Array();
var my_indices    = new Array();
var fensterbreite = new Array();
var fensterhoehe  = new Array();
 
my_regexps[0]   = new RegExp('(http\\:\\/\\/)?(\\w+\\.)*youtube\\.com/watch\\?[\\w\\=\\&\\-]*v\\=([\\w\\-]*)', 'i');
my_indices[0]   = 3;
my_regexps[1]   = new RegExp('(http\\:\\/\\/)?video.google(\\.\\w+)+/(\\w+)\\?docid\\=([\\-\\d]+)', 'i');
my_indices[1]   = 4;
my_regexps[2]   = new RegExp('(http\\:\\/\\/)?(\\w+\\.)*rutube\\.ru\\/tracks\\/\\d+\\.html\\?[\\w\\=\\&\\-]*v\\=([\\w\\-]*)', 'i');
my_indices[2]   = 3;
my_regexps[3]   = new RegExp('(http\\:\\/\\/)?(\\w+\\.)*myvideo\\.de\\/watch\\/(\\d+)[\\?\\w\\=\\&\\-]*', 'i');
my_indices[3]   = 3;
my_regexps[4]   = new RegExp('(http\\:\\/\\/)?video.golem(\\.\\w+)+/(\\w+)\\/([\\-\\d]+)', 'i');
my_indices[4]   = 4;
my_regexps[5]   = new RegExp('(http\\:\\/\\/)?(\w+\\.)*clipfish\\.de(\\/\\w+\\.\\w+\\/\\w+\\/\\d+\\/\\w+\\/)(\\d+)', 'i');
my_indices[5]   = 4;
my_regexps[6]   = new RegExp('(http\\:\\/\\/)?(\w+\\.)*metacafe\\.com/watch\\/([a-zA-Z0-9._% -]*\\/\\w+)(\\/)', 'i');
my_indices[6]   = 3;

if(popupfenster == 0) {
  my_templates[0] = '<object id="'+idname+'video" width="'+breit+'" height="'+hoch+'">'
                    +'<param name="movie" value="http://youtube.com/swf/l.swf?video_id=THISisTHEvideoID" />'
                    +'<param name="wmode" value="transparent" />'
                    +'<param name="AllowFullscreen" value="true" />'
                    +'<embed id="'+idname+'video" src="http://youtube.com/swf/l.swf?video_id=THISisTHEvideoID" type="application/x-shockwave-flash" wmode="transparent" width="'+breit+'" height="'+hoch+'"></embed></object>';
  
  my_templates[1] = '<object id="'+idname+'video" width="'+breit+'" height="'+hoch+'">'
                    +'<param name="movie" value="http://video.google.com/googleplayer.swf?docId=THISisTHEvideoID"/>'
                    +'<param name="wmode" value="transparent" />'
                    +'<param name="AllowFullscreen" value="true" />'
                    +'<embed id="'+idname+'video" src="http://video.google.com/googleplayer.swf?docId=THISisTHEvideoID" type="application/x-shockwave-flash" wmode="transparent" width="'+breit+'" height="'+hoch+'"></embed></object>';
  
  my_templates[2] = '<OBJECT id="'+idname+'video" width="'+breit+'" height="'+hoch+'">'
                    +'<PARAM name="movie" value="http://video.rutube.ru/THISisTHEvideoID">'
                    +'</PARAM><PARAM name="wmode" value="window">'
                    +'</PARAM><PARAM name="allowFullScreen" value="true"></PARAM>'
                    +'<EMBED id="'+idname+'video" src="http://video.rutube.ru/THISisTHEvideoID" type="application/x-shockwave-flash" wmode="window" width="'+breit+'" height="'+hoch+'" allowFullScreen="true" ></EMBED></OBJECT>';
  
  my_templates[3] = '<object id="'+idname+'video" width="'+breit+'" height="'+hoch+'">'
                    +'<param name="movie" value="http://www.myvideo.de/movie/THISisTHEvideoID"/>'
                    +'<param name="wmode" value="transparent" />'
                    +'<param name="AllowFullscreen" value="true" />'
                    +'<embed id="'+idname+'video" src="http://www.myvideo.de/movie/THISisTHEvideoID" type="application/x-shockwave-flash" wmode="transparent" width="'+breit+'" height="'+hoch+'"></embed></object>';
   
  my_templates[4] = '<object id="'+idname+'video" width="'+breit+'" height="'+hoch+'">'
                    +'<param name="movie" value="http://video.golem.de/player/videoplayer.swf?id=THISisTHEvideoID&autoPl=false"></param>'
                    +'<param name="allowFullScreen" value="true"></param>'
                    +'<param name="AllowScriptAccess" value="always">'
                    +'<embed id="'+idname+'video" src="http://video.golem.de/player/videoplayer.swf?id=THISisTHEvideoID&autoPl=false" type="application/x-shockwave-flash" allowfullscreen="true" AllowScriptAccess="always" width="'+breit+'" height="'+hoch+'"></embed></object>';

  my_templates[5] = '<object id="'+idname+'" classid="clsid:d27cdb6e-ae6d-11cf-96b8-444553540000" codebase="http://fpdownload.macromedia.com/pub/shockwave/cabs/flash/swflash.cab#version=8,0,0,0" width="'+breit+'" height="'+hoch+'" id="player" align="middle">'
                    +'<param name="allowScriptAccess" value="always" />'
                    +'<param name="movie" value="http://www.clipfish.de/videoplayer.swf?as=0&vid=THISisTHEvideoID&r=1" />'
                    +'<param name="quality" value="high" />'
                    +'<param name="bgcolor" value="#FFF" />'
                    +'<param name="allowFullScreen" value="true" />'
                    +'<embed src="http://www.clipfish.de/videoplayer.swf?as=0&videoid=THISisTHEvideoID&r=1" quality="high" bgcolor="#FFF" width="'+breit+'" height="'+hoch+'" name="player" align="middle" allowFullScreen="true" allowScriptAccess="always" type="application/x-shockwave-flash" pluginspage="http://www.macromedia.com/go/getflashplayer">'
                    +'</embed></object>';

  my_templates[6]  = '<embed id="'+idname+'video" src="http://www.metacafe.com/fplayer/THISisTHEvideoID.swf" width="'+breit+'" height="'+hoch+'" wmode="transparent" pluginspage="http://www.macromedia.com/go/getflashplayer" type="application/x-shockwave-flash"></embed>';
  
}
else {
  my_templates[0] = "http://youtube.com/swf/l.swf?video_id=THISisTHEvideoID"; fensterbreite[0] = "425"; fensterhoehe[0] = "344";
  my_templates[1] = "http://video.google.com/googleplayer.swf?docId=THISisTHEvideoID"; fensterbreite[1] = "400"; fensterhoehe[1] = "326";
  my_templates[2] = "http://video.rutube.ru/THISisTHEvideoID"; fensterbreite[2] = "470"; fensterhoehe[2] = "353"; 
  my_templates[3] = "http://www.myvideo.de/movie/THISisTHEvideoID"; fensterbreite[3] = "470"; fensterhoehe[3] = "406";
  my_templates[4] = "http://video.golem.de/player/videoplayer.swf?id=THISisTHEvideoID"; fensterbreite[4] = "480"; fensterhoehe[4] = "270";
  my_templates[5] = "http://www.clipfish.de/videoplayer.swf?as=0&vid=THISisTHEvideoID"; fensterbreite[5] = "464"; fensterhoehe[5] = "380";
  my_templates[6] = "http://www.metacafe.com/fplayer/THISisTHEvideoID.swf";  fensterbreite[6] ="498", fensterhoehe[6] = "423";
}
  
for (var my_counter = 0; my_counter < my_regexps.length; my_counter++) {
  var my_regexp   = my_regexps[my_counter];    
  var my_template = my_templates[my_counter];
  var my_index    = my_indices[my_counter];
  var re_result = tidy_message.match(my_regexp);
  if (re_result == null) {
    continue;
  }
  if (popupfenster == 0) {
    text_to_display = my_template.replace(/THISisTHEvideoID/g,re_result[my_index]);
    return (text_to_display); 
  }
  else {
    file = my_template.replace(/THISisTHEvideoID/g,re_result[my_index]);
    description = 'video'; 
    properties  = 'width='+fensterbreite[my_counter]+', height='+fensterhoehe[my_counter]+', left=25, TOP=50, LOCATION=0, MENUBAR=0, RESIZABLE=0, SCROLLBARS=0, STATUS=0, directories=0'; 
    window.open(file,description,properties);
    return "";
  }
 }
}

// ico

var imagespfad   = ToolPfad+'icons/'  
var seitenverhaeltnis = proportion;
document.write('<div id="mausbereich" width="1" height="1" style=\"overflow:hidden; position:absolute; z-index:175;\"></div>');

function linkconvert(text,bildumwandlung,videoumwandlung,swfumwandlung,seitenumwandlung) {
 einstellungspruefung_convert(imagespfad,1,'imagespfad','config.js','\"DeinPfad/\"');   
 einstellungspruefung_convert(vorschaubuttonstatus,2,'vorschaubuttonstatus','config.js','Icons vor und hinter dem Link mit 1=an und 0=aus\n\nReihenfolge: Bildlink, URLBildlink, Videolink und SWFlink'); 
 einstellungspruefung_convert(vorschaupopup,3,'vorschaupopup','config.js','Vorschaupopupbilder mit 1=an und 0=aus\n\nReihenfolge: Bildlink und URLBildlink');  
  
 text = text.replace(/<br>/g,' yyx '); 
  
  hrefsplit = text.split('<a class=\"link\" target=\"_self\"');
  ausgabe = hrefsplit[0];
  for (j=1; j<hrefsplit.length;j++) {
    teiltext = hrefsplit[j].split("</a>");
    gefunden="undefiniert";
    link = teiltext[0].split(">")[1];
      
    if ((videoumwandlung == 'yes') && (gefunden=="undefiniert") && (window.videoplay)) {
      var VideoAnbieter = new Array();
      VideoAnbieter[0] = "(http\\:\\/\\/)?(\\w+\\.)*youtube\\.com/watch\\?[\\w\\=\\&\\-]*v\\=([\\w\\-]*)";
      VideoAnbieter[1] = "(http\\:\\/\\/)?video.google(\\.\\w+)+/(\\w+)\\?docid\\=([\\-\\d]+)";
      VideoAnbieter[2] = "(http\\:\\/\\/)?rutube.ru\\/tracks\\/\\d+\\.*html\\?[\\w\\=\\&\\*v=\\w\\-]";
      VideoAnbieter[3] = "(http\\:\\/\\/)?(\\w+\\.)*myvideo\\.de\\/watch\\/(\\d+)[\\?\\w\\=\\&\\-]*";
      VideoAnbieter[4] = "(http\\:\\/\\/)?video.golem.de\\/?[$ a-zA-Z0-9_ -]";
      VideoAnbieter[5] = "(http\\:\\/\\/)?(\w+\\.)*clipfish\\.de(\\/\\w+\\.\\w+\\/\\w+\\/\\d+\\/\\w+\\/\\d+)";
      VideoAnbieter[6] = "(http\\:\\/\\/)?(\w+\\.)*metacafe\\.com/watch\\/(\\w+)\\/?[a-zA-Z0-9_-]";
      for (index=0; index<VideoAnbieter.length; index++) {
        if ( teiltext[0].search(VideoAnbieter[index]) != -1 ) {
          bildlink = screenbildersteller(link);
          kurzerlink = linkverkuerzer(link)
          gefunden="video";
        }
      }
    }

    if ((bildumwandlung == 'yes') && (gefunden=="undefiniert")) {
      BildEndungen = new Array(".bmp",".gif",".jpg",".png",".xbm",".jpeg");
      for (index=0; index<BildEndungen.length; index++) {
        if (((link.substr(link.length-4,4)).toLowerCase()==BildEndungen[index]) || ((link.substr(link.length-5,5)).toLowerCase()==BildEndungen[index])) {
          screenshotlink = link;
          kurzerlink = linkverkuerzer(link);
          switch (vorschaupopup.charAt(0)) {
            case "0":
              bildlink = "";
            break;
            case "1":
              bildlink = link;
            break;
          }
          gefunden="bild";
        }
      }
    }
  
    if ((swfumwandlung == 'yes') && (gefunden=="undefiniert")) {
      FlashEndungen = new Array(".swf","admin");
      for (index=0; index<FlashEndungen.length; index++) {
        if (((link.substr(link.length-4,4)).toLowerCase()==FlashEndungen[index]) || ((link.substr(link.length-6,6)).toLowerCase()==FlashEndungen[index])) {
          screenshotlink = link;
          kurzerlink = linkverkuerzer(link);
          bildlink = link;
          gefunden="swf";
        }
      }
    }
    
    if ((seitenumwandlung == 'yes') && (gefunden=="undefiniert")) {
      switch (vorschaupopup.charAt(1)) {
        case "0":
          bildlink = "";
        break;
        case "1":
          bildlink = screenbildersteller(link);
        break;
      }
      kurzerlink = linkverkuerzer(link)
      gefunden="urlbild";  
    }
    
    if (gefunden=="undefiniert") {
      link = "<a "+teiltext[0]+"</a>";
    }
    
    var name = Math.floor(Math.random()*9999) 
    switch (gefunden) {
      case "bild":
        teiltext0ersatz = "";
        if (vorschaubuttonstatus.charAt(0)==1) { teiltext0ersatz = teiltext0ersatz+'<img src="'+imagespfad+'picture_go.png" class="icons" onmouseover="src=\''+imagespfad+'picture_go_d.png\';" onmouseout="src=\''+imagespfad+'picture_go.png\';" onmousedown="picturearea(document.getElementById(\''+name+'div\'),this.alt)" title="Picture area indicate / fade out" alt="'+link+'[trenn]'+bildlink+'[trenn]'+name+'">'; }
        teiltext0ersatz = teiltext0ersatz+'<a href="'+link+'" class="Link-Text" onmousemove="mausdrauf(this.alt);" onmouseout="this.className=\'Link-Text\';mausrunter();" onmouseover="this.className=\'Link-Text2\';mausdrauf(this.alt);" alt="'+link+'[trenn]'+bildlink+'[trenn]'+name+'">'+kurzerlink+'</a>';
        if (vorschaubuttonstatus.charAt(1)==1) { teiltext0ersatz = teiltext0ersatz+'<img src="'+imagespfad+'picture_go.png" class="icons" onmouseover="src=\''+imagespfad+'picture_go_d.png\';" onmouseout="src=\''+imagespfad+'picture_go.png\';" onmousedown="picturearea(document.getElementById(\''+name+'div\'),this.alt)" title="Picture area indicate / fade out" alt="'+link+'[trenn]'+bildlink+'[trenn]'+name+'">'; }
        teiltext0ersatz = teiltext0ersatz+' '+'<div id="'+name+'div" style="display:none" class="imagebox"></div>'; 
      break;
      case "urlbild":
        teiltext0ersatz = "";
        if (vorschaubuttonstatus.charAt(2)==1) { teiltext0ersatz = teiltext0ersatz+'<img src="'+imagespfad+'favicon.png" class="icons" onmouseover="src=\''+imagespfad+'favicon_d.png\';" onmouseout="src=\''+imagespfad+'favicon.png\';" onmousedown="picturearea(document.getElementById(\''+name+'div\'),this.alt)" title="Picture area indicate / fade out" alt="'+link+'[trenn]'+bildlink+'[trenn]'+name+'">'; }
        teiltext0ersatz = teiltext0ersatz+'<a href="'+link+'" class="Link-Text" onmousemove="mausdrauf(this.alt);" onmouseout="this.className=\'Link-Text\';mausrunter();" onmouseover="this.className=\'Link-Text2\';mausdrauf(this.alt);" alt="'+link+'[trenn]'+bildlink+'[trenn]'+name+'">'+kurzerlink+'</a>';
        if (vorschaubuttonstatus.charAt(3)==1) { teiltext0ersatz = teiltext0ersatz+'<img src="'+imagespfad+'favicon.png" class="icons" onmouseover="src=\''+imagespfad+'favicon_d.png\';" onmouseout="src=\''+imagespfad+'favicon.png\';" onmousedown="picturearea(document.getElementById(\''+name+'div\'),this.alt)" title="Picture area indicate / fade out" alt="'+link+'[trenn]'+bildlink+'[trenn]'+name+'">'; }
        teiltext0ersatz = teiltext0ersatz+' '+'<div id="'+name+'div" style="display:none" class="imagebox"></div>';
      break;
      case "video":
        var Video_Bild = "./VideoPl"
        var Video_Bild_Endung = ".png"
        teiltext0ersatz = "";
        if (vorschaubuttonstatus.charAt(4)==1) {
          teiltext0ersatz = teiltext0ersatz+'<img src="'+imagespfad+'\\'+Video_Bild+'2'+Video_Bild_Endung+'" class="icons" onMouseOver="src=\''+imagespfad+''+Video_Bild+'2_d'+Video_Bild_Endung+'\'" onMouseOut="src=\''+imagespfad+''+Video_Bild+'2'+Video_Bild_Endung+'\'" onmousedown="videoarea(document.getElementById(\''+name+'div\'),this.alt,1)" title="Videoarea indicate / fade out" alt="'+link+'[trenn]'+name+'">';
          teiltext0ersatz = teiltext0ersatz+'<img src="'+imagespfad+'\\'+Video_Bild+'1'+Video_Bild_Endung+'" class="icons" onMouseOver="src=\''+imagespfad+''+Video_Bild+'1_d'+Video_Bild_Endung+'\'" onMouseOut="src=\''+imagespfad+''+Video_Bild+'1'+Video_Bild_Endung+'\'" onmousedown="videoarea(document.getElementById(\''+name+'div\'),this.alt,0)" title="Videoarea indicate / fade out" alt="'+link+'[trenn]'+name+'">';
          teiltext0ersatz = teiltext0ersatz+' ';    
        }
        teiltext0ersatz = teiltext0ersatz+'<a href='+link+' class="Link-Text" onmousemove="mausdrauf(this.alt);" onmouseout="this.className=\'Link-Text\';mausrunter();" onmouseover="this.className=\'Link-Text2\';mausdrauf(this.alt);" alt="'+link+'[trenn]'+bildlink+'[trenn]'+name+'">'+link+'</a>';
        if (vorschaubuttonstatus.charAt(5)==1) {
          teiltext0ersatz = teiltext0ersatz+'<img src="'+imagespfad+'\\'+Video_Bild+'2'+Video_Bild_Endung+'" class="icons" onMouseOver="src=\''+imagespfad+''+Video_Bild+'2_d'+Video_Bild_Endung+'\'" onMouseOut="src=\''+imagespfad+''+Video_Bild+'2'+Video_Bild_Endung+'\'" onmousedown="videoarea(document.getElementById(\''+name+'div\'),this.alt,1)" title="Videoarea indicate / fade out" alt="'+link+'[trenn]'+name+'">';
          teiltext0ersatz = teiltext0ersatz+'<img src="'+imagespfad+'\\'+Video_Bild+'1'+Video_Bild_Endung+'" class="icons" onMouseOver="src=\''+imagespfad+''+Video_Bild+'1_d'+Video_Bild_Endung+'\'" onMouseOut="src=\''+imagespfad+''+Video_Bild+'1'+Video_Bild_Endung+'\'" onmousedown="videoarea(document.getElementById(\''+name+'div\'),this.alt,0)" title="Videoarea indicate / fade out" alt="'+link+'[trenn]'+name+'">';
          teiltext0ersatz = teiltext0ersatz+' ';    
        }
        teiltext0ersatz = teiltext0ersatz+' '+'<div id="'+name+'div" class="videobox"></div>';
      break;
      case "swf":
        teiltext0ersatz = "";
        if (vorschaubuttonstatus.charAt(6)==1) { teiltext0ersatz = teiltext0ersatz+'<img src="'+imagespfad+'show.png" class="icons" onmouseover="src=\''+imagespfad+'show_d.png\';" onmouseout="src=\''+imagespfad+'show.png\'" onmousedown="swfarea(document.getElementById(\''+name+'div\'),this.alt)" title="Animation area indicate / fade out" alt="'+link+'[trenn]'+name+'">'; }
        teiltext0ersatz = teiltext0ersatz+'<a href="'+link+'" class="Link-Text" onmouseout="this.className=\'Link-Text\'" onmouseover="this.className=\'Link-Text2\';">'+kurzerlink+'</a>';
        if (vorschaubuttonstatus.charAt(7)==1) { teiltext0ersatz = teiltext0ersatz+'<img src="'+imagespfad+'show.png" class="icons" onmouseover="src=\''+imagespfad+'show_d.png\';" onmouseout="src=\''+imagespfad+'show.png\'" onmousedown="swfarea(document.getElementById(\''+name+'div\'),this.alt)" title="Animation area indicate / fade out" alt="'+link+'[trenn]'+name+'">'; }
        teiltext0ersatz = teiltext0ersatz+' '+'<div id="'+name+'div" class="swfbox"></div>';
      break;
      default:
        teiltext0ersatz = link;
      break;
    }
    ausgabe = ausgabe + teiltext0ersatz + teiltext[1];
  }
  ausgabe = ausgabe.replace(/ yyx /g,'<br>'); 
  return ausgabe;
}

// global f

function dateitypiconermittlung(dateiendung) {
  dateiendung = dateiendung.toLowerCase();    
  switch (dateiendung) {
  case "7z":   icon = 'Archive/7zip'; break;
  case "rar":  icon = 'Archive/rar';  break;
  case "zip":  icon = 'Archive/zip';  break;        
  case "mp3":  icon = 'Audio/mp3'; break;
  case "mid":  icon = 'Audio/mid'; break;
  case "ogg":  icon = 'Audio/ogg'; break;
  case "wav":  icon = 'Audio/wav'; break;
  case "wma":  icon = 'Audio/wma'; break;                      
  case "bmp":  icon = 'Image/bmp';  break;
  case "gif":  icon = 'Image/gif';  break;
  case "jpg":  icon = 'Image/jpg';  break;
  case "jpeg": icon = 'Image/jpeg'; break;    
  case "png":  icon = 'Image/png';  break;  
  case "tif":  icon = 'Image/tif';  break;                           
  case "csv":  icon = 'Office/csv'; break;       
  case "doc":  icon = 'Office/doc';  break;    
  case "docx": icon = 'Office/docx'; break;  
  case "mdb":  icon = 'Office/mdb';  break;
  case "mdbx": icon = 'Office/mdbx'; break;    
  case "pdf":  icon = 'Office/pdf';  break;     
  case "ppt":  icon = 'Office/ppt';  break;
  case "pptx": icon = 'Office/pptx'; break;          
  case "vsd":  icon = 'Office/vsd';  break;
  case "xls":  icon = 'Office/xls';  break;
  case "xlsm": icon = 'Office/xlsx'; break;         
  case "xlsx": icon = 'Office/xlsx'; break;                                                     
  case "avi":  icon = 'Video/avi';   break;
  case "divx": icon = 'Video/divx';   break;  
  case "flv":  icon = 'Video/flv';   break;        
  case "mkv":  icon = 'Video/mkv';   break;  
  case "mp4":  icon = 'Video/mp4';   break;              
  case "mpg":  icon = 'Video/mpg';   break;     
  case "mpeg": icon = 'Video/mpeg';  break; 
  case "mov":  icon = 'Video/mov';   break;                 
  case "wmv":  icon = 'Video/wmv';   break; 
  case "asp":  icon = 'System/asp';  break;   
  case "bat":  icon = 'System/bat';  break;
  case "bin":  icon = 'System/bin';  break;   
  case "css":  icon = 'System/css';  break; 
  case "cue":  icon = 'System/cue';  break;
  case "exe":  icon = 'System/exe';  break;        
  case "htm":  icon = 'System/htm';  break;   
  case "html": icon = 'System/html'; break;      
  case "ini":  icon = 'System/ini';  break;
  case "iso":  icon = 'System/iso';  break;
  case "nfo":  icon = 'System/nfo';  break;
  case "psd":  icon = 'System/psd';  break;   
  case "txt":  icon = 'System/txt';  break;
  case "xml":  icon = 'System/xml';  break;              
  default:  icon = 'System/default'; break;                                             
  }
  
  return icon;
 }

function getavatar(avatar){
  if( avatar.indexOf('.swf',0)!=-1) {
    document.write('<object width="48" height="60" name="movie" classid="clsid:D27CDB6E-AE6D-11cf-96B8-444553540000" codebase="http://download.macromedia.com/pub/shockwave/cabs/flash/swflash.cab#version=5,0,0,0" class="headeravaflash">'
    + '<param name=movie value=' + avatar + '>'
    + '<param name=quality value=high>'
    + '<embed src=' + avatar + ' quality=high pluginspage="http://www.macromedia.com/shockwave/download/index.cgi?P1_Prod_Version=ShockwaveFlash" type="application/x-shockwave-flash" class="headeravaflash">'
    + '</embed></object>');
  }
  else {
    document.write('<img src="' + avatar + '" width="48" />');
  }
}

function linkverkuerzer(orilink) {
var kurzerlink;
if ((linkverkuerzen.toLowerCase()!="yes") && (linkverkuerzen.toLowerCase()!="ja")) {
  kurzerlink = orilink;
}
else {
  kurzerlink = orilink.replace(/(\/\/[a-zA-Z0-9._% -]*\/)([a-zA-Z0-9._% -~\/]*)(\/[a-zA-Z0-9._% -~]*)/g,'$1...$3');
  kurzerlink = kurzerlink.replace(/(\/\/[a-zA-Z0-9._% -~:\/]*).php?([a-zA-Z0-9._% -~:\/]*)/g,'$1');
}
return kurzerlink;
}

function mausdrauf(ids) {
  if ((vorschaubildview.toLowerCase()=="yes") || (vorschaubildview.toLowerCase()=="ja")) {
    idlink = ids.split('[trenn]')[0];
    idbildlink = ids.split('[trenn]')[1];
    if (idbildlink) {
      idname = ids.split('[trenn]')[2];
      if (typeof document.compatMode != 'undefined' && document.compatMode != 'BackCompat') {
        scrollPos     = document.documentElement.scrollTop;
        Fensterhoehe  = document.documentElement.offsetHeight;
        Fensterbreite = document.documentElement.offsetWidth;
        ScrollbalkenY = 2;
        ScrollbalkenX = 2;
      }
      else {
        if (typeof document.body != 'undefined') {
          if(document.getElementById){
            scrollPos     = document.getElementById("Body").scrollTop;
            Fensterhoehe  = document.getElementById("Body").offsetHeight;
            Fensterbreite = document.getElementById("Body").offsetWidth;
            ScrollbalkenY = 20;
            ScrollbalkenX = 20;
          }
          else {
            scrollPos     = document.all.body.scrollTop;
            Fensterhoehe  = document.all.body.offsetHeight;
            Fensterbreite = document.all.body.offsetWidth;
            ScrollbalkenY = 20;
            ScrollbalkenX = 20;
          }
        }
      }
      document.getElementById("mausbereich").innerHTML='<img id="bildvorschau" class="bildvorschau" src="'+idbildlink+'">'; 

      if (vorschaubildposiY==-1) {
        if (window.event.clientY + document.getElementById("bildvorschau").height+ScrollbalkenY+10 > Fensterhoehe) {
          YY = scrollPos+Fensterhoehe-document.getElementById("bildvorschau").height-ScrollbalkenY;
        }
        else {
          YY = scrollPos+window.event.clientY+10;
        }
      }
      else {
        YY = scrollPos+(Fensterhoehe-document.getElementById("bildvorschau").height-ScrollbalkenY)/100*vorschaubildposiY;
      }
      
      if (vorschaubildposiX==-1) {
        if (window.event.clientX + document.getElementById("bildvorschau").width+ScrollbalkenX+10 > Fensterbreite) {
          XX = Fensterbreite-document.getElementById("bildvorschau").width-ScrollbalkenX;
        }
        else {
          XX = window.event.clientX+10;
        }
      }
      else {
        XX = (Fensterbreite-document.getElementById("bildvorschau").width-ScrollbalkenX)/100*vorschaubildposiX;
      }
      
      document.getElementById("mausbereich").style.top  = YY;
      document.getElementById("mausbereich").style.left = XX;
    }
  }
}

function mausrunter() {
  document.getElementById("mausbereich").innerHTML="";
}

function otherarea(iddiv,ids) {
  if (iddiv.innerHTML!="") {
    iddiv.innerHTML="";
    iddiv.style.display="none";
  }
  else {
    idlink = ids.split('[trenn]')[0];
    ausgabe = '<br>&nbsp;&nbsp;&nbsp;<a class="Link-Text" onmouseout="this.className=\'Link-Text\';" onmouseover="this.className=\'Link-Text2\';" href="'+idlink+'">'+idlink+'</a><br><br>';
    iddiv.innerHTML=ausgabe;
    iddiv.style.display="block";
  }
}  

function picturearea(bilderdiv,ids) {
  if (bilderdiv.innerHTML!="") {
    bilderdiv.innerHTML="";
    bilderdiv.style.display="none";
  }
  else {
    idlink = ids.split('[trenn]')[0];
    idbildlink = ids.split('[trenn]')[1];
    idname = ids.split('[trenn]')[2];
    ausgabe = '&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;'
             +'<img src="'+imagespfad+'/arrow_turn_left.png" class="icons" onmouseover="src=\''+imagespfad+'/arrow_turn_left_d.png\';" onmouseout="src=\''+imagespfad+'/arrow_turn_left.png\'" onmousedown="getElementById(\''+idname+'i\').style.filter=\'progid:DXImageTransform.Microsoft.BasicImage(grayscale=0, xray=0, mirror=0, invert=0, opacity=1, rotation=3);\'" title="Nach links gedreht">'
             +'<img src="'+imagespfad+'/arrow_up.png" class="icons" onmouseover="src=\''+imagespfad+'/arrow_up_d.png\';" onmouseout="src=\''+imagespfad+'/arrow_up.png\'" onmousedown="getElementById(\''+idname+'i\').style.filter=\'progid:DXImageTransform.Microsoft.BasicImage(grayscale=0, xray=0, mirror=0, invert=0, opacity=1, rotation=4)\'" title="Standardausrichtung">'
             +'<img src="'+imagespfad+'/arrow_turn_right.png" class="icons" onmouseover="src=\''+imagespfad+'/arrow_turn_right_d.png\';" onmouseout="src=\''+imagespfad+'/arrow_turn_right.png\'" onmousedown="getElementById(\''+idname+'i\').style.filter=\'progid:DXImageTransform.Microsoft.BasicImage(grayscale=0, xray=0, mirror=0, invert=0, opacity=1, rotation=1)\'" title="Nach rechts gedreht">&nbsp;&nbsp;&nbsp;';
    ausgabe = ausgabe+'<img src="'+imagespfad+'/zoom_in.png" class="icons" onmouseover="src=\''+imagespfad+'/zoom_in_d.png\';" onmouseout="src=\''+imagespfad+'/zoom_in.png\'" onmousedown="resize_big(document.getElementById(\''+idname+'i\'))" title="Vergr&ouml;&szlig;ern">'
                     +'<img src="'+imagespfad+'/zoom_out.png" class="icons" onmouseover="src=\''+imagespfad+'/zoom_out_d.png\';" onmouseout="src=\''+imagespfad+'/zoom_out.png\'" onmousedown="resize_small(document.getElementById(\''+idname+'i\'))" title="Verkleinern"><br>';
    ausgabe = ausgabe+'<a href='+idlink+'><img id="'+idname+'i" class="imagebox" onmouseout="this.className=\'imagebox\'" onmouseover="this.className=\'imagebox2\'" src="'+idbildlink+'" alt="'+idlink+'" onload="resize(this)"></a><br>';
    bilderdiv.innerHTML=ausgabe;
    bilderdiv.style.display="block";
  }
}

function resize_animation(bereich,veraengerungsart) { 
  bereich.display = "inline"; 
  switch (veraengerungsart) {
  case "vergroessern": 
    if ((bereich.height<3200) && (bereich.width<3200)) {
      bereich.height = (bereich.height * 1.1) + "px"; 
      bereich.width  = (bereich.width  * 1.1) + "px"; 
    }
  break;
  case "verkleinern": 
    if ((bereich.height<3200) && (bereich.width<3200)) {
      bereich.height = (bereich.height / 1.1) + "px"; 
      bereich.width  = (bereich.width  / 1.1) + "px"; 
    }
  break;
  case "seitenverhaeltnisgroesser":
    if (seitenverhaeltnis < 20/9) {
      seitenverhaeltnis += 1/18;
      bereich.width = bereich.width * seitenverhaeltnis / (seitenverhaeltnis-1/18);
    }
  break;
  case "seitenverhaeltnisreset":
    seitenverhaeltnis = proportion;
    bereich.width = bereich.height * seitenverhaeltnis;
  break;
  case "seitenverhaeltniskleiner":
    if (seitenverhaeltnis > 9/9) {
      seitenverhaeltnis -= 1/18;
      bereich.width = bereich.width * seitenverhaeltnis / (seitenverhaeltnis+1/18);
    }
  break;
  }
} 

function resize(img) { 
  img.style.display = "inline"; 
  if(img.width > maxwidth) {
    img.style.height = (img.height * maxwidth / img.width) + "px"; 
    img.style.width = maxwidth+'px'; 
  } 
  if(img.height > maxheight) {
    img.style.width = (img.width * maxheight / img.height) + "px"; 
    img.style.height = maxheight+'px';
  }
}  

function resize_big(img) { 
  img.style.display = "inline"; 
  if ((img.height<3200) && (img.width<3200)) {
    img.style.height = (img.height * 1.1) + "px"; 
    img.style.width  = (img.width  * 1.1) + "px"; 
  }
} 

function resize_small(img) { 
  img.style.display = "inline"; 
  if ((img.height>10) && (img.width>10)) { 
    img.style.height = (img.height / 1.1) + "px"; 
    img.style.width  = (img.width  / 1.1) + "px"; 
  }
} 

function rotate(img) {  
  imgRotation = new Array();
  if (!imgRotation[obj.src]) {
    imgRotation[obj.src] = 1;
  }
  else {
    imgRotation[obj.src]++;
  }      
  obj.style.filter = 'progid:DXImageTransform.Microsoft.BasicImage(rotation='+(imgRotation[obj.src]%4)+')';
}

function screenbildersteller(link) {
  var anbieter = '\"1\" -> http://images.websnapr.com\n\"2\" -> http://ss.puxz.com\n\"3\" -> http://screen.puxz.com\n\"4\" -> http://www.artviper.net\n\"5\" -> http://www.shrinktheweb.com   (ACHTUNG: Benötigt Account)';
  einstellungspruefung_globalfunctions(makeurlscreenanbieter,1,'makeurlscreenanbieter','config.js',anbieter);
  einstellungspruefung_globalfunctions(anbieterpasswort,2,'anbieterpasswort','config.js','Bitte geben Sie ein Passwort an!');
  switch (makeurlscreenanbieter) {
    case "1":
      screenshotlink = 'http://images.websnapr.com/?url='+link;
    break;
    case "2":
      screenshotlink = 'http://ss.puxz.com//'+link;
    break;
    case "3":
      screenshotlink = 'http://screen.puxz.com/1024x768/'+link;
    break;
    case "4":
      screenshotlink = 'http://www.artviper.net/screenshots/screener.php?url='+link+'&w=200&q=80';
    break;
    case "5":
      screenshotlink = 'http://www.shrinktheweb.com/xino.php?embed=1&stwu=ca504&STWAccessKeyId='+anbieterpasswort+'&Size=xlg&stwUrl='+link;  // Benötigt Passwort
    break;
    default:
    break;
  }
  return screenshotlink;
}

function swfarea(swfflash,ids) { 
  if (typeof document.compatMode != 'undefined' && document.compatMode != 'BackCompat') {
    swfbereich = swfflash.style;
    hoch  = document.documentElement.offsetHeight;
    breit = document.documentElement.offsetWidth;
  }
  else {
    if (typeof document.body != 'undefined') {
      if(document.getElementById){
        swfbereich = swfflash.style;
        if (document.getElementById("Body").offsetWidth>document.getElementById("Body").offsetHeight){
          hoch  = document.getElementById("Body").offsetHeight * 3/4;
          breit = hoch * seitenverhaeltnis;
        }
        else {
          breit = document.getElementById("Body").offsetWidth * 3/4;
          hoch  = breit * 1/seitenverhaeltnis;
        }
      }
      else {
        if (document.getElementById("Body").offsetWidth>document.getElementById("Body").offsetHeight){
          hoch  = document.all.body.offsetHeight * 3/4;
          breit = hoch * seitenverhaeltnis;
        }
        else {
          breit = document.all.body.offsetWidth * 3/4;
          hoch  = breit * 1/seitenverhaeltnis;
        }
      }
    }
  }
  idlink = ids.split('[trenn]')[0];
  idname = ids.split('[trenn]')[1];
  if (swfflash.innerHTML!="") {
    swfflash.innerHTML="";
    swfbereich.display = 'none';
  }
  else {
    ausgabe = '<img src="'+imagespfad+'zoom_in.png" class="icons" onmouseover="src=\''+imagespfad+'zoom_in_d.png\';" onmouseout="src=\''+imagespfad+'zoom_in.png\'" onmousedown="resize_animation(document.getElementById(\''+idname+'swf\'),\'vergroessern\')" title="Vergr&ouml;&szlig;ern">'
             +'<img src="'+imagespfad+'zoom_out.png" class="icons" onmouseover="src=\''+imagespfad+'zoom_out_d.png\';" onmouseout="src=\''+imagespfad+'zoom_out.png\'" onmousedown="resize_animation(document.getElementById(\''+idname+'swf\'),\'verkleinern\')" title="Verkleinern">&nbsp;&nbsp;&nbsp;';
    ausgabe = ausgabe+'<img src="'+imagespfad+'breiter.png" class="icons" onmouseover="src=\''+imagespfad+'breiter_d.png\';" onmouseout="src=\''+imagespfad+'breiter.png\'" onmousedown="resize_animation(document.getElementById(\''+idname+'swf\'),\'seitenverhaeltnisgroesser\')" title="Seitenverh&auml;ltnis vergr&ouml;&szlig;ern">'
                     +'<img src="'+imagespfad+'reset.png" class="icons" onmouseover="src=\''+imagespfad+'/reset_d.png\';" onmouseout="src=\''+imagespfad+'/reset.png\'" onmousedown="resize_animation(document.getElementById(\''+idname+'swf\'),\'seitenverhaeltnisreset\')" title="Seitenverh&auml;ltnis wiederherstellen">'
                     +'<img src="'+imagespfad+'schmaler.png" class="icons" onmouseover="src=\''+imagespfad+'/schmaler_d.png\';" onmouseout="src=\''+imagespfad+'/schmaler.png\'" onmousedown="resize_animation(document.getElementById(\''+idname+'swf\'),\'seitenverhaeltniskleiner\')" title="Seitenverh&auml;ltnis verkleinern"><br>';
    ausgabe = ausgabe+'<object id="'+idname+'swf" width="'+breit+'" height="'+hoch+'" classid="CLSID:D27CDB6E-AE6D-11cf-96B8-444553540000" codebase="http://active.macromedia.com/flash2/cabs/swflash.cab#version=4,0,0,0">';
    ausgabe = ausgabe+'<param name="movie" value="'+idlink+'">';
    ausgabe = ausgabe+'</object>';
    swfflash.innerHTML=ausgabe;
    swfbereich.display = 'block';
  }
}

function videoarea(videos,ids,popup) {
  if (typeof document.compatMode != 'undefined' && document.compatMode != 'BackCompat') {
      videobereich = videos.style;
      hoch  = document.documentElement.offsetHeight * 3/4;
      breit = document.documentElement.offsetWidth * 3/4;
  }
  else {
    if (typeof document.body != 'undefined') {
      if(document.getElementById){
        videobereich = videos.style;
        if (document.getElementById("Body").offsetWidth>document.getElementById("Body").offsetHeight){
          hoch  = document.getElementById("Body").offsetHeight * 3/4;
          breit = hoch * seitenverhaeltnis;
        }
        else {
          breit = document.getElementById("Body").offsetWidth * 3/4;
          hoch  = breit * 1/seitenverhaeltnis;
        }
      }
      else {
        if (document.getElementById("Body").offsetWidth>document.getElementById("Body").offsetHeight){
          hoch  = document.all.body.offsetHeight * 3/4;
          breit = hoch * seitenverhaeltnis;
        }
        else {
          breit = document.all.body.offsetWidth * 3/4;
          hoch  = breit * 1/seitenverhaeltnis;
        }
      }
    }
  }
  idlink = ids.split('[trenn]')[0];
  idname = ids.split('[trenn]')[1];
  if (Videobutton!=2) {
    popup = Videobutton;
  }
  if (popup == 1) {
    videoplay(idlink,idname,hoch,breit,popup);
  }
  else {  
    if (videos.innerHTML!="") {
      if (videobereich.display == 'none') {
        videobereich.display = 'block';
      }
      else {
        videobereich.display = 'none';
      }
    }
    else {
      var videostring = videoplay(idlink,idname,hoch,breit,popup);
      if (videostring!=0) {
        ausgabe = '<img src="'+imagespfad+'/zoom_in.png" class="icons" onmouseover="src=\''+imagespfad+'/zoom_in_d.png\';" onmouseout="src=\''+imagespfad+'/zoom_in.png\'" onmousedown="resize_animation(document.getElementById(\''+idname+'video\'),\'vergroessern\')" title="Vergr&ouml;&szlig;ern">'
                 +'<img src="'+imagespfad+'/zoom_out.png" class="icons" onmouseover="src=\''+imagespfad+'/zoom_out_d.png\';" onmouseout="src=\''+imagespfad+'/zoom_out.png\'" onmousedown="resize_animation(document.getElementById(\''+idname+'video\'),\'verkleinern\')" title="Verkleinern">&nbsp;&nbsp;&nbsp;';
        ausgabe = ausgabe+'<img src="'+imagespfad+'/breiter.png" class="icons" onmouseover="src=\''+imagespfad+'/breiter_d.png\';" onmouseout="src=\''+imagespfad+'/breiter.png\'" onmousedown="resize_animation(document.getElementById(\''+idname+'video\'),\'seitenverhaeltnisgroesser\')" title="Seitenverh&auml;ltnis vergr&ouml;&szlig;ern">'
                         +'<img src="'+imagespfad+'/reset.png" class="icons" onmouseover="src=\''+imagespfad+'/reset_d.png\';" onmouseout="src=\''+imagespfad+'/reset.png\'" onmousedown="resize_animation(document.getElementById(\''+idname+'video\'),\'seitenverhaeltnisreset\')" title="Seitenverh&auml;ltnis wiederherstellen">'
                         +'<img src="'+imagespfad+'/schmaler.png" class="icons" onmouseover="src=\''+imagespfad+'/schmaler_d.png\';" onmouseout="src=\''+imagespfad+'/schmaler.png\'" onmousedown="resize_animation(document.getElementById(\''+idname+'video\'),\'seitenverhaeltniskleiner\')" title="Seitenverh&auml;ltnis verkleinern"><br>';
        ausgabe = ausgabe+videoplay(idlink,idname,hoch,breit,popup);
        videobereich.display = 'block';
      }
      else {
        ausgabe = "";
        videobereich.display = 'none';
      }
      videos.innerHTML=ausgabe;
    }
  }
}



// data

function getandsenddata(eingangsmeldung,chatpartner,basispfad,uinchatpartner,cid) {    

  einstellungspruefung_showdata(iconpfad,1,'iconpfad','config.js','\"DeinPfad/\"'); 
  einstellungspruefung_showdata(empfangsdateipfad,2,'empfangsdateipfad','config.js','\"X:/DeinPfad/\" oder \"X:\\\\DeinPfad\\\\\"'); 

  switch (empfangsdateipfad.substr(1,2)) {
  case ':\\': case ':/':
   mirandapfad = 'file://' + empfangsdateipfad;
  break;
  default:
   einstellungspruefung_showdata(empfangsdateipfad,3,'empfangsdateipfad','config.js','\"/DeinPfad/\" oder \"\\\\DeinPfad\\\\\"');  
   
   mirandapfad_save  = basispfad.split(/[Mm][Ii][Rr][Aa][Nn][Dd][Aa]/);  
   if(mirandapfad_save[1]) {  
     mirandapfad2_save = mirandapfad_save[1].split('/');
     mpfad_length = mirandapfad_save[0].length + mirandapfad2_save[0].length + 7;
     mirandapfad = basispfad.substr(0,mpfad_length) + empfangsdateipfad;   
   }       
   else {
     drive_search = basispfad.search(/\W[A-Za-z]\W\W/);       
     drivesearch_length = drive_search + 3; 
     mirandapfad = basispfad.substr(0,drivesearch_length) + empfangsdateipfad;    
   }
  }
  
  einstellungspruefung_showdata(unterverzeichnis,4,'unterverzeichnis','config.js','\"1\" -> Unterordner mit Kontaktlistenname des Chatpartners\n\"2\" -> Unterordner mit UIN des Chatpartners\n\"3\" -> Alle Dateien in einem Ordner');
  
  switch (unterverzeichnis) {
    case "1":
      chatpartnerordner = mirandapfad+chatpartner;
    break;
    case "2":
      chatpartnerordner = mirandapfad+uinchatpartner;
    break;
    case "3":
      chatpartnerordner = mirandapfad.substr(0,--mirandapfad.length);
    break;    
    default:
      chatpartnerordner = "";
    break;
  }

  aeshellsplit    = eingangsmeldung.split('.AESHELL');
  eingangsmeldung = aeshellsplit[0];
  
  emeldung     = eingangsmeldung.split(':'); 
  var randomid = Math.floor(Math.random()*9999) 
  var gefunden = false;
    
  if(eingangsmeldung.lastIndexOf('.') == -1) {
    transfertyp = 'multi';
    if (cid != 0) {
      transfertyp += 'eingehend';
    }
    else {
      transfertyp += 'ausgehend';
    } 
  }  
  else {
   if (emeldung[1]) {
     transfertyp = 'ausgehend';
   }
   else {
     transfertyp = 'eingehend'; 
   }
  }
  
  switch (transfertyp) {
  case 'ausgehend':
    pfad = emeldung[0]+':'+emeldung[1]; 
    var dateipfadarray = pfad.split("\\");
    var dateipfadposition = 0;
    var dateipfad = "";
    var dateinameposition = dateipfadarray.length;   
    var dateiname = dateipfadarray[--dateinameposition];
    while (dateipfadposition < dateinameposition) {
     dateipfad += dateipfadarray[dateipfadposition]+'\\';
     dateipfadposition++; 
    }
    var datei = dateipfad + dateiname;
    dateipfadkurz = dateipfad.replace(/([a-zA-Z]:)\\[a-zA-Z0-9._% -~:]*\\([a-zA-Z0-9._% -~:]*)[\\]/g,'$1\\...\\$2');  
    document.write('<div id="'+randomid+'button" class="imagebox_filetransfer"></div>&nbsp;');
    document.write('File [<a class="Link-Text" onmouseout="this.className=\'Link-Text\';" onmouseover="this.className=\'Link-Text2\';" href="'+datei +'" title="'+ datei +'">'+ dateiname +'</a>] from recorder [<a class="Link-Text" onmouseout="this.className=\'Link-Text\';" onmouseover="this.className=\'Link-Text2\';" href="'+ dateipfad +'" title="'+ dateipfad +'">'+ dateipfadkurz +'</a>] sent.');   
  break;
  case 'eingehend':
    chatpartnerordner = chatpartnerordner.substring(7,chatpartnerordner.length);  
    chatpartnerordner = chatpartnerordner.replace(/\//g, '\\');    
    var datei = chatpartnerordner+'\\'+emeldung;
    var dateiname = emeldung; 
    document.write('<div id="'+randomid+'button" class="imagebox_filetransfer"></div>&nbsp;');
    document.write('File [<a class="Link-Text" onmouseout="this.className=\'Link-Text\';" onmouseover="this.className=\'Link-Text2\';" href="'+datei +'" title="'+datei +'">'+ dateiname +'</a>] from ');
    document.write('<a class="Link-Text" onmouseout="this.className=\'Link-Text\';" onmouseover="this.className=\'Link-Text2\';" class="textlink" href="'+chatpartnerordner+'" title="'+chatpartnerordner+'">'+chatpartner+' ('+uinchatpartner+')</a> are transfered or ready for reseption. ');
  break;
  case 'multiausgehend':
    chatpartnerordner = chatpartnerordner.substring(7,chatpartnerordner.length);  
    chatpartnerordner = chatpartnerordner.replace(/\//g, '\\'); 
    var datei = chatpartnerordner+'\\';   
    var dateianzahl = emeldung; 
    document.write('<div id="'+randomid+'button" class="imagebox_filetransfer"></div>&nbsp;');
    document.write(dateianzahl +' for ');
    document.write('<a class="Link-Text" onmouseout="this.className=\'Link-Text\';" onmouseover="this.className=\'Link-Text2\';" class="textlink" href="'+chatpartnerordner+'" title="'+chatpartnerordner+'">'+chatpartner+' ('+uinchatpartner+')</a> are sent. ');
  break;  
  case 'multieingehend':
    chatpartnerordner = chatpartnerordner.substring(7,chatpartnerordner.length);  
    chatpartnerordner = chatpartnerordner.replace(/\//g, '\\');
    var datei = chatpartnerordner+'\\';  
    var dateianzahl = emeldung; 
    dateianzahl = dateianzahl[0].replace('Files','Dateien');    
    document.write('<div id="'+randomid+'button" class="imagebox_filetransfer"></div>&nbsp;');
    document.write(dateianzahl +' for ');
    document.write('<a class="Link-Text" onmouseout="this.className=\'Link-Text\';" onmouseover="this.className=\'Link-Text2\';" class="textlink" href="'+chatpartnerordner+'" title="'+chatpartnerordner+'">'+chatpartner+' ('+uinchatpartner+')</a> ');   
  break;
  default:
  break;
  }

  var endungarray = datei.split(".");
  var endungposition = endungarray.length-1;
  var dateiendung = endungarray[endungposition];  
  iconart = dateitypiconermittlung(dateiendung);
  icon_d = iconart+ '_d' + '.png'; 
  icon = iconart+ '.png';
  bildendungen = new Array(".JPG",".jpg",".BMP",".bmp",".PNG",".png",".GIF",".gif",".JPEG",".jpeg");
  for (i=0; i<bildendungen.length; i++) {
    if (eingangsmeldung.search(bildendungen[i]) != -1 ) {
      bildpfad = datei;
      ausgabe = '<img src="'+iconpfad+icon+'" class="icons_filetypes" onmouseover="src=\''+iconpfad+icon_d+'\';" onmouseout="src=\''+iconpfad+icon+'\';" onmousedown="picturearea(document.getElementById(\''+randomid+'pic\'),this.alt)" title="Picture area indicate / fade out" alt="'+datei+'[trenn]'+bildpfad+'[trenn]'+randomid+'">';
      document.getElementById(randomid+'button').innerHTML=ausgabe;
      ausgabe = '<div id="'+randomid+'pic" class="imagebox2_filetransfer"></div>';
      document.write(ausgabe);
      gefunden = true;
    }
  }
  
  if (gefunden==false) {
     ausgabe = '<img src="'+iconpfad+icon+'" class="icons_filetypes" onmouseover="src=\''+iconpfad+icon_d+'\';" onmouseout="src=\''+iconpfad+icon+'\';" onmousedown="otherarea(document.getElementById(\''+randomid+'other\'),this.alt)" title="Bereich anzeigen/ausblenden" alt="'+ datei + '">';
     document.getElementById(randomid+'button').innerHTML=ausgabe;
     ausgabe = '<div id="'+randomid+'other" class="otherbox"></div>';
     document.write(ausgabe);
  }  
}


// tzer

var tzeranimpfad = ToolPfad+'tZersFiles/animation/'; 
var tzerbildpfad = ToolPfad+'tZersFiles/grafik/';  
<!--
document.write('<img src="'+tzerbildpfad+'close.png" class="icons" onmouseover="src=\''+tzerbildpfad+'/close_d.png\';" onmouseout="src=\''+tzerbildpfad+'/close.png\';" style=\"height:32px; width:32px; position:absolute; visibility:hidden; z-index:30000;\" id="closeButton" onclick="ausblenden()" title="close" alt="Close">');
document.write('<div id="tZersDiv" align=\"right\" style=\" margin-top: 2px; margin-left: 2px; overflow:hidden; position:absolute; top:0; visibility:hidden; z-index:200;\"></div>');
TimeoutClear = 0;  

function ausblenden() {
  document.getElementById("tZersDiv").style.visibility = "hidden";
  document.getElementById("closeButton").style.visibility = "hidden";
  document.getElementById("tZersDiv").innerHTML = "";
}

function neubestimmen() {
  tZersHoehenAnpassung  = 4;
  tZersBreitenAnpassung = 21; 
  var scrollPos;
  var bildbreite;
  var bildhoehe;
  if (typeof document.compatMode != 'undefined' && document.compatMode != 'BackCompat') {
    scrollPos  = document.documentElement.scrollTop;
    bildbreite = document.documentElement.offsetWidth;
    bildhoehe  = document.documentElement.offsetHeight;
  }
  else 
    if (typeof document.body != 'undefined') {
      if(document.getElementById){
        scrollPos  = document.getElementById("Body").scrollTop;
        bildbreite = document.getElementById("Body").offsetWidth;
        bildhoehe  = document.getElementById("Body").offsetHeight;
      }
      else {
        scrollPos  = document.all.body.scrollTop;
        bildbreite = document.all.body.offsetWidth;
        bildhoehe  = document.all.body.offsetHeight;
      }
    }
  document.getElementById("closeButton").style.top = scrollPos+7;
  document.getElementById("closeButton").style.left = bildbreite - tZersBreitenAnpassung - document.getElementById("closeButton").style.width.substr(0,2) - 3;
  
  document.getElementById("tZersDiv").style.height = bildhoehe - tZersHoehenAnpassung;
  document.getElementById("tZersDiv").style.width  = bildbreite - tZersBreitenAnpassung;
  document.getElementById("tZersDiv").style.left = 0;
  document.getElementById("tZersDiv").style.top = scrollPos;
  
  if (document.getElementById("tZersDiv").innerHTML != "") { 
    tzerzoom(100);
  }

}

function tzerzoom(grundwert) {
  document.getElementById("tZersObject").Zoom(0);
  switch (swfname) {
    case "gangsta":   case 'gangsterSheep': wert = grundwert - 40; break;
    case "canthearu": case 'cant_hear':     wert = grundwert - 40; break;
    case "skratch":   case 'scratch':       wert = grundwert - 40; break;
    case "boo":                             wert = grundwert - 10; break;
    case "kisses":                          wert = grundwert - 25; break;
    case "chillout":  case 'rastamab':      wert = grundwert - 15; break;
    case "akitaka":   case 'sappuko':       wert = grundwert - 40; break;
    case "laugh":                           wert = grundwert - 50; break;
    case "duh":       case 'dahh':          wert = grundwert - 00; break;
    case "beback":    case 'beBack':        wert = grundwert - 15; break;
    case "likeu":     case 'iLikeU':        wert = grundwert - 40; break;
    case "sorry":                           wert = grundwert - 58; break;
  }
  document.getElementById("tZersObject").Zoom(wert);
}

function tzersabspielen(swflink) {
  setInterval("neubestimmen()", 1);
  document.getElementById("tZersDiv").innerHTML =
  '<object id="tZersObject" classid="CLSID:D27CDB6E-AE6D-11cf-96B8-444553540000" Width=100% height=100% border=0 codebase="http://active.macromedia.com/flash2/cabs/swflash.cab#version=4,0,0,0">'
  +'<param name="movie" value="'+swflink+'">'
  +'<PARAM NAME=wmode VALUE=transparent>'
  +'</object>';
  window.clearTimeout(TimeoutClear);
  TimeoutClear = window.setTimeout("ausblenden();",15000);
  document.getElementById("tZersObject").StopPlay();
  document.getElementById("tZersDiv").style.visibility = "visible";
  document.getElementById("closeButton").style.visibility = "visible";
  document.getElementById("tZersObject").Play();
}

function tZersOutput (eingangsmeldung,chatpartner,tzersoderbild,zeigetzersname,bekommensenden) {
  einstellungspruefung_tzerausgabe(tzeranimpfad,1,'tzeranimpfad','config.js','\"DeinPfad/\"'); 
  einstellungspruefung_tzerausgabe(tzerbildpfad,1,'tzerbildpfad','config.js','\"DeinPfad/\"'); 

  link = eingangsmeldung;
  link = link.split("swf");
  if (link[1] && (link[0].substr(0,4)=="tZer" || link[0].substr(54,5)=="xtraz" || link[0].substr(55,5)=="xtraz")) {
    link = link[1].split("http://");
     
    swfname = link[1].substring(link[1].lastIndexOf('/')+1,link[1].length-1);
    bildlink= tzerbildpfad+swfname+'.png';
    swflink = tzeranimpfad+swfname+'.swf';
    tzersname = eingangsmeldung;
    if ((eingangsmeldung.substr(54,5)=='xtraz') || (eingangsmeldung.substr(55,5)=='xtraz')) {
      tzersname =  tzersname.split('.');
      tzersname =  tzersname[2].substring(tzersname[2].lastIndexOf('/')+1,tzersname[2].length)+'';
      switch (tzersname) {
        case "gangsta":   case 'gangsterSheep': tzersname = "Gangsta";       break;
        case "canthearu": case 'cant_hear':     tzersname = "Can't Hear U";  break;
        case "skratch":   case 'scratch':       tzersname = "Scratch";       break;
        case "boo":                             tzersname = "Booooo";        break;
        case "kisses":                          tzersname = "Kisses";        break;
        case "chillout":  case 'rastamab':      tzersname = "Chill Out!";    break;
        case "akitaka":   case 'sappuko':       tzersname = "Akitaka";       break;
        case "sorry":                           tzersname = "I'm Sorry";     break;
        case "laugh":                           tzersname = "Hilaaarious";   break;
        case "duh":       case 'dahh':          tzersname = "Like Duh!";     break;
        case "beback":    case 'beBack':        tzersname = "L8R";           break;
        case "likeu":     case 'iLikeU':        tzersname = "Like U!";       break;
        default: break;
      }
    }
    else {
      icqversion = "2";
      switch (icqversion) {
        case "1":
          tzersname = tzersname[1].split('"');
          tzersname = tzersname[0].split('freeData');
          tzersname = tzersname[0].split('url');
          tzersname = tzersname[0].split('"');
          tzersname = tzersname[0].substring(7,tzersname[0].length-7);
        break;
        case "2":
          tzersname = tzersname.split("''");
          tzersname = tzersname[0].split('"');
          tzersname = tzersname[0].split('http://');
          tzersname = tzersname[0].substring(1,tzersname[0].length-13);
        break;
        case "3":
        break;
        default:
        break;
      }
    } 
      
    if (zeigetzersname == 'yes') {
      if (bekommensenden == 'get') {
        document.write(chatpartner+' tZers : <font class="tzerlink" onmouseout="this.className=\'tzerlink\'" onmouseover="this.className=\'tzerlink2\'" onclick="tzersabspielen(&quot;'+swflink+'&quot;)">'+tzersname+'</font>');
      }  
      else {
        document.write('You tZer with <font class="tzerlink" onmouseout="this.className=\'tzerlink\'" onmouseover="this.className=\'tzerlink2\'" onclick="tzersabspielen(&quot;'+swflink+'&quot;)">'+tzersname+'</font>');
      }
    }

    if (tzersoderbild=="tzers" || tzersoderbild=="both") {
      tzersabspielen(swflink);
    }
    if (tzersoderbild=="picture" || tzersoderbild=="both") {
      document.write('<div class="tzerbildbereich">');
      document.write('<img class="tzerbild" src="'+bildlink+'" alt="'+tzersname+'" title="'+tzersname+'" onclick="tzersabspielen(&quot;'+swflink+'&quot;)">'); 
      document.write('</div>');
    }
    return eingangsmeldung.split("</a>")[1];
  }

  else {
    return eingangsmeldung;
  }
}

//validateconfig

function standardfehlertext(variablenname,skriptname,optionen) {
fehlermeldung  = ' ist fehlerhaft definiert.\n';
fehlermeldung += 'Bitte korrigieren Sie diesen Fehler in der Variable \"'+variablenname+'\" der Datei \"'+skriptname+'\".\n\n';
fehlermeldung += 'Die Variable stellt folgende Optionen bereit:\n' + optionen; 
return fehlermeldung  
}

function einstellungspruefung_convert(pruefvariable,variante,variablenname,skriptname,optionen) {
   switch(variante) {
   case 1:
    pruefvariable = pruefvariable.substr(ToolPfad.length,pruefvariable.length);
    if (pruefvariable.search(/^\W[A-Za-z]/) > -1 || pruefvariable.search(/[A-Za-z]\W$/) == -1) {
          fehlermeldung = 'Ein Pfad' + standardfehlertext(variablenname,skriptname,optionen);    
          alert(fehlermeldung);
    }         
   break;
   case 2:
    if (pruefvariable.search(/[0-1][0-1][0-1][0-1][0-1][0-1][0-1][0-1]/) == -1) {
          fehlermeldung = 'Eine Einstellung' + standardfehlertext(variablenname,skriptname,optionen);    
          alert(fehlermeldung);
    }         
   break;   
   case 3:
    if (pruefvariable.search(/[0-1][0-1]/) == -1) {
          fehlermeldung = 'Eine Einstellung' + standardfehlertext(variablenname,skriptname,optionen);    
          alert(fehlermeldung);
    }         
   break;    
   default:
   break;   
   }
} 

function einstellungspruefung_clock(pruefvariable,variante,variablenname,skriptname,optionen) {
   switch(variante) {
   case 1:
   
   break;
   default:
   break;   
   }
} 

function einstellungspruefung_globalfunctions(pruefvariable,variante,variablenname,skriptname,optionen) {
   switch(variante) {
   case 1:
    if (pruefvariable.search(/[1-5]/) == -1) {
          fehlermeldung = 'Eine Einstellung' + standardfehlertext(variablenname,skriptname,optionen);    
          alert(fehlermeldung);
    }         
   break;
   case 2:
    if (makeurlscreenanbieter.search(/[5]/) != -1 && pruefvariable.length == 0) {
          fehlermeldung = 'Eine Einstellung' + standardfehlertext(variablenname,skriptname,optionen);    
          alert(fehlermeldung);
    }         
   break;   
   default:
   break;   
   }
} 


function einstellungspruefung_showdata(pruefvariable,variante,variablenname,skriptname,optionen) {
   switch(variante) {
   case 1:
    pruefvariable = pruefvariable.substr(ToolPfad.length,pruefvariable.length);
    if (pruefvariable.search(/^\W[A-Za-z]/) > -1 || pruefvariable.search(/[A-Za-z]\W$/) == -1) {
          fehlermeldung = 'Ein Pfad' + standardfehlertext(variablenname,skriptname,optionen);    
          alert(fehlermeldung);
    }      
   break;
   case 2:
    if (pruefvariable.search(/^[A-Za-z]\W[A-Za-z]/) >= 0) {
          fehlermeldung = 'Ein Pfad' + standardfehlertext(variablenname,skriptname,optionen);
          alert(fehlermeldung);
        }   
   break;    
   case 3:
    if (pruefvariable.search(/^\W[A-Za-z]/) == -1 || pruefvariable.search(/[A-Za-z]\W$/) == -1) {
          fehlermeldung = 'Ein Pfad' + standardfehlertext(variablenname,skriptname,optionen);    
          alert(fehlermeldung);
    }      
   break;
   case 4:
    if (pruefvariable.search(/[1-3]/) == -1) {
          fehlermeldung = 'Eine Einstellung' + standardfehlertext(variablenname,skriptname,optionen);    
          alert(fehlermeldung);
    }      
   break;  
   default:
   // !
   break;   
   }
} 

function einstellungspruefung_tzerausgabe(pruefvariable,variante,variablenname,skriptname,optionen) {
   switch(variante) {
   case 1:
    pruefvariable = pruefvariable.substr(ToolPfad.length,pruefvariable.length);
    if (pruefvariable.search(/^\W[A-Za-z]/) > -1 || pruefvariable.search(/[A-Za-z]\W$/) == -1) {
          fehlermeldung = 'Ein Pfad' + standardfehlertext(variablenname,skriptname,optionen);    
          alert(fehlermeldung);
    }         
   break;
   default:
   break;   
   }
} 

function einstellungspruefung_videos(pruefvariable,variante,variablenname,skriptname,optionen) {
   switch(variante) {
   case 1:
    pruefvariable = pruefvariable.toString();
    if (pruefvariable.search(/[0-2]/) == -1) {
          fehlermeldung = 'Eine Einstellung' + standardfehlertext(variablenname,skriptname,optionen);    
          alert(fehlermeldung);
    }       
   break;
   default:
   break;   
   }
} 

// parser

function parser(message) {
  
  message = message.replace(/http\:\/\/www.abload.de\/image.php\?img\=/g, 'http://www.abload.de/img/');
  message = message.replace(/([a-zA-Z0-9_\-\.]+)@((\[[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.)|(([a-zA-Z0-9\-]+\.)+))([a-zA-Z]{2,4}|[0-9]{1,3})(\]?)(?=(\s|<|$))/g, '<img src="img/mail.png" width="19px" hight="17" class="icon"><a href="mailto:$&"><span style="color:#999999; text-decoration: underline;">$&</span></a>');
  message = message.replace(/&lt;DIV&gt;/g,'');
  message = message.replace(/&lt;\/DIV&gt;/g,'');
  return message;
}

