
function mailru(text)
{
 if (text.indexOf("&lt;SMILE&gt;id=flas")!= -1)
 {
 	if (text.indexOf('id=flash_25')!= -1)
	{
	    document.write("<img src='!tools/tZersFiles/mailru/PNG/tank.png'/><p><i>Прорвемся!</i><p>");
	}
	else if (text.indexOf('id=flash_24')!= -1)
	{
	    document.write("<img src='!tools/tZersFiles/mailru/PNG/missyou.png'/><p><i>Скучаю</i><p>");
	}
	else if (text.indexOf('id=flash_23')!= -1)
	{
	    document.write("<img src='!tools/tZersFiles/mailru/PNG/krizis.png'/><p><i>Голяк</i><p>");
	}
	else if (text.indexOf('id=flash_22')!= -1)
	{
	    document.write("<img src='!tools/tZersFiles/mailru/PNG/joy.png'/><p><i>Радость</i><p>");
	}
	else if (text.indexOf('id=flash_21')!= -1)
	{
	    document.write("<img src='!tools/tZersFiles/mailru/PNG/information.png'/><p><i>Взрыв мозга</i><p>");
	}
	else if (text.indexOf('id=flash_20')!= -1)
	{
	    document.write("<img src='!tools/tZersFiles/mailru/PNG/bad_cold.png'/><p><i>Болею!</i><p>");
	}
    else if (text.indexOf('id=flash_17')!= -1)
	{
	tzerfile = '!tools\\tZersFiles\\mailru\\kot_wow.swf';
        document.write("<img src='!tools/tZersFiles/mailru/PNG/kot_wow.png'/><p><i>Ух ты!</i><p>");		
		document.write('<center><object width=50% height=50% border=0 name="movie" classid="clsid:D27CDB6E-AE6D-11cf-96B8-444553540000" codebase="http://download.macromedia.com/pub/shockwave/cabs/flash/swflash.cab#version=9,0,28,0" width="720" height="576">\
				<param name="movie" value="' + tzerfile + '">\
				<param name="quality" value="high">\
				<param name="scale" value="showall">\
				<param name="wmode" value="transparent">\
				<embed src="' + tzerfile + '" quality="high" pluginspage="http://www.adobe.com/shockwave/download/download.cgi?P1_Prod_Version=ShockwaveFlash" wmode="transparent" type="application/x-shockwave-flash" width="720" height="576">\
				</embed></object></center></div>');
	}
	
    else if (text.indexOf('id=flash_16')!= -1)
	{
		tzerfile = '!tools\\tZersFiles\\mailru\\kot_obida.swf';
        document.write("<img src='!tools/tZersFiles/mailru/PNG/kot_obida.png'/><p><i>Обидели</i><p>");		
		document.write('<center><object width=50% height=50% border=0 name="movie" classid="clsid:D27CDB6E-AE6D-11cf-96B8-444553540000" codebase="http://download.macromedia.com/pub/shockwave/cabs/flash/swflash.cab#version=9,0,28,0" width="720" height="576">\
				<param name="movie" value="' + tzerfile + '">\
				<param name="quality" value="high">\
				<param name="scale" value="showall">\
				<param name="wmode" value="transparent">\
				<embed src="' + tzerfile + '" quality="high" pluginspage="http://www.adobe.com/shockwave/download/download.cgi?P1_Prod_Version=ShockwaveFlash" wmode="transparent" type="application/x-shockwave-flash" width="720" height="576">\
				</embed></object></center></div>');
	}
	else if (text.indexOf('id=flash_15')!= -1)
	{
		tzerfile = '!tools\\tZersFiles\\mailru\\kot_nedutza.swf';
		document.write("<img src='!tools/tZersFiles/mailru/PNG/kot_nedutza.png'/><p><i>Не дуться!</i><p>");
		document.write('<center><object width=50% height=50% border=0 name="movie" classid="clsid:D27CDB6E-AE6D-11cf-96B8-444553540000" codebase="http://download.macromedia.com/pub/shockwave/cabs/flash/swflash.cab#version=9,0,28,0" width="720" height="576">\
				<param name="movie" value="' + tzerfile + '">\
				<param name="quality" value="high">\
				<param name="scale" value="showall">\
				<param name="wmode" value="transparent">\
				<embed src="' + tzerfile + '" quality="high" pluginspage="http://www.adobe.com/shockwave/download/download.cgi?P1_Prod_Version=ShockwaveFlash" wmode="transparent" type="application/x-shockwave-flash" width="720" height="576">\
				</embed></object></center></div>');
	}
	else if (text.indexOf('id=flash_14')!= -1)
	{
		tzerfile = '!tools\\tZersFiles\\mailru\\kot_spasibo.swf';
		document.write("<img src='!tools/tZersFiles/mailru/PNG/kot_spasibo.png'/><p><i>Спасибо!</i><p>");
		document.write('<center><object width=50% height=50% border=0 name="movie" classid="clsid:D27CDB6E-AE6D-11cf-96B8-444553540000" codebase="http://download.macromedia.com/pub/shockwave/cabs/flash/swflash.cab#version=9,0,28,0" width="720" height="576">\
				<param name="movie" value="' + tzerfile + '">\
				<param name="quality" value="high">\
				<param name="scale" value="showall">\
				<param name="wmode" value="transparent">\
				<embed src="' + tzerfile + '" quality="high" pluginspage="http://www.adobe.com/shockwave/download/download.cgi?P1_Prod_Version=ShockwaveFlash" wmode="transparent" type="application/x-shockwave-flash" width="720" height="576">\
				</embed></object></center></div>');
	}
	else if (text.indexOf('id=flash_13')!= -1)
	{
		tzerfile = '!tools\\tZersFiles\\mailru\\kot_goodbye.swf';
		document.write("<img src='!tools/tZersFiles/mailru/PNG/kot_goodbye.png'/><p><i>Пока-пока</i><p>");
		document.write('<center><object width=50% height=50% border=0 name="movie" classid="clsid:D27CDB6E-AE6D-11cf-96B8-444553540000" codebase="http://download.macromedia.com/pub/shockwave/cabs/flash/swflash.cab#version=9,0,28,0" width="720" height="576">\
				<param name="movie" value="' + tzerfile + '">\
				<param name="quality" value="high">\
				<param name="scale" value="showall">\
				<param name="wmode" value="transparent">\
				<embed src="' + tzerfile + '" quality="high" pluginspage="http://www.adobe.com/shockwave/download/download.cgi?P1_Prod_Version=ShockwaveFlash" wmode="transparent" type="application/x-shockwave-flash" width="720" height="576">\
				</embed></object></center></div>');
	}
	else if (text.indexOf('id=flash_12')!= -1)
	{
		tzerfile = '!tools\\tZersFiles\\mailru\\kot_cool.swf';
		document.write("<img src='!tools/tZersFiles/mailru/PNG/kot_cool.png'/><p><i>Круто!</i><p>");
		document.write('<center><object width=50% height=50% border=0 name="movie" classid="clsid:D27CDB6E-AE6D-11cf-96B8-444553540000" codebase="http://download.macromedia.com/pub/shockwave/cabs/flash/swflash.cab#version=9,0,28,0" width="720" height="576">\
				<param name="movie" value="' + tzerfile + '">\
				<param name="quality" value="high">\
				<param name="scale" value="showall">\
				<param name="wmode" value="transparent">\
				<embed src="' + tzerfile + '" quality="high" pluginspage="http://www.adobe.com/shockwave/download/download.cgi?P1_Prod_Version=ShockwaveFlash" wmode="transparent" type="application/x-shockwave-flash" width="720" height="576">\
				</embed></object></center></div>');
	}
	else if (text.indexOf('id=flash_11')!= -1)
	{
		tzerfile = '!tools\\tZersFiles\\mailru\\rabotaet.swf';
		document.write("<img src='!tools/tZersFiles/mailru/PNG/mult_working.png'/><p><i>Я занят</i><p>");
		document.write('<center><object width=50% height=50% border=0 name="movie" classid="clsid:D27CDB6E-AE6D-11cf-96B8-444553540000" codebase="http://download.macromedia.com/pub/shockwave/cabs/flash/swflash.cab#version=9,0,28,0" width="720" height="576">\
				<param name="movie" value="' + tzerfile + '">\
				<param name="quality" value="high">\
				<param name="scale" value="showall">\
				<param name="wmode" value="transparent">\
				<embed src="' + tzerfile + '" quality="high" pluginspage="http://www.adobe.com/shockwave/download/download.cgi?P1_Prod_Version=ShockwaveFlash" wmode="transparent" type="application/x-shockwave-flash" width="720" height="576">\
				</embed></object></center></div>');
	}
	else if (text.indexOf('id=flash_10')!= -1)
	{
		tzerfile = '!tools\\tZersFiles\\mailru\\sobaka_strelyaet.swf';
		document.write("<img src='!tools/tZersFiles/mailru/PNG/mult_shoot.png'/><p><i>Пристрелю!</i><p>");
		document.write('<center><object width=50% height=50% border=0 name="movie" classid="clsid:D27CDB6E-AE6D-11cf-96B8-444553540000" codebase="http://download.macromedia.com/pub/shockwave/cabs/flash/swflash.cab#version=9,0,28,0" width="720" height="576">\
				<param name="movie" value="' + tzerfile + '">\
				<param name="quality" value="high">\
				<param name="scale" value="showall">\
				<param name="wmode" value="transparent">\
				<embed src="' + tzerfile + '" quality="high" pluginspage="http://www.adobe.com/shockwave/download/download.cgi?P1_Prod_Version=ShockwaveFlash" wmode="transparent" type="application/x-shockwave-flash" width="720" height="576">\
				</embed></object></center></div>');
	}
	else if (text.indexOf('id=flash_9')!= -1)
	{
		tzerfile = '!tools\\tZersFiles\\mailru\\rosy.swf';
		document.write("<img src='!tools/tZersFiles/mailru/PNG/mult_roses.png'/><p><i>Миллион алых роз!</i><p>");
		document.write('<center><object width=50% height=50% border=0 name="movie" classid="clsid:D27CDB6E-AE6D-11cf-96B8-444553540000" codebase="http://download.macromedia.com/pub/shockwave/cabs/flash/swflash.cab#version=9,0,28,0" width="720" height="576">\
				<param name="movie" value="' + tzerfile + '">\
				<param name="quality" value="high">\
				<param name="scale" value="showall">\
				<param name="wmode" value="transparent">\
				<embed src="' + tzerfile + '" quality="high" pluginspage="http://www.adobe.com/shockwave/download/download.cgi?P1_Prod_Version=ShockwaveFlash" wmode="transparent" type="application/x-shockwave-flash" width="720" height="576">\
				</embed></object></center></div>');
	}
	else if (text.indexOf('id=flash_8')!= -1)
	{
		tzerfile = '!tools\\tZersFiles\\mailru\\mad_dog.swf';
		document.write("<img src='!tools/tZersFiles/mailru/PNG/mult_maddog.png'/><p><i>А я сошла с ума...</i><p>");
		document.write('<center><object width=50% height=50% border=0 name="movie" classid="clsid:D27CDB6E-AE6D-11cf-96B8-444553540000" codebase="http://download.macromedia.com/pub/shockwave/cabs/flash/swflash.cab#version=9,0,28,0" width="720" height="576">\
				<param name="movie" value="' + tzerfile + '">\
				<param name="quality" value="high">\
				<param name="scale" value="showall">\
				<param name="wmode" value="transparent">\
				<embed src="' + tzerfile + '" quality="high" pluginspage="http://www.adobe.com/shockwave/download/download.cgi?P1_Prod_Version=ShockwaveFlash" wmode="transparent" type="application/x-shockwave-flash" width="720" height="576">\
				</embed></object></center></div>');
	}
else if (text.indexOf('id=flash_7')!= -1)
	{
		tzerfile = '!tools\\tZersFiles\\mailru\\smeh.swf';
		document.write("<img src='!tools/tZersFiles/mailru/PNG/mult_laugh.png'/><p><i>Жжёшь!</i><p>");
		document.write('<center><object width=50% height=50% border=0 name="movie" classid="clsid:D27CDB6E-AE6D-11cf-96B8-444553540000" codebase="http://download.macromedia.com/pub/shockwave/cabs/flash/swflash.cab#version=9,0,28,0" width="720" height="576">\
				<param name="movie" value="' + tzerfile + '">\
				<param name="quality" value="high">\
				<param name="scale" value="showall">\
				<param name="wmode" value="transparent">\
				<embed src="' + tzerfile + '" quality="high" pluginspage="http://www.adobe.com/shockwave/download/download.cgi?P1_Prod_Version=ShockwaveFlash" wmode="transparent" type="application/x-shockwave-flash" width="720" height="576">\
				</embed></object></center></div>');
	}
	else if (text.indexOf('id=flash_6')!= -1)
	{
		tzerfile = '!tools\\tZersFiles\\mailru\\serdze.swf';
		document.write("<img src='!tools/tZersFiles/mailru/PNG/mult_heart.png'/><p><i>Сердце</i><p>");
		document.write('<center><object width=50% height=50% border=0 name="movie" classid="clsid:D27CDB6E-AE6D-11cf-96B8-444553540000" codebase="http://download.macromedia.com/pub/shockwave/cabs/flash/swflash.cab#version=9,0,28,0" width="720" height="576">\
				<param name="movie" value="' + tzerfile + '">\
				<param name="quality" value="high">\
				<param name="scale" value="showall">\
				<param name="wmode" value="transparent">\
				<embed src="' + tzerfile + '" quality="high" pluginspage="http://www.adobe.com/shockwave/download/download.cgi?P1_Prod_Version=ShockwaveFlash" wmode="transparent" type="application/x-shockwave-flash" width="720" height="576">\
				</embed></object></center></div>');
	}
	else if (text.indexOf('id=flash_5')!= -1)
	{
		tzerfile = '!tools\\tZersFiles\\mailru\\bodun.swf';
		document.write("<img src='!tools/tZersFiles/mailru/PNG/mult_hangover.png'/><p><i>Отходняк...</i><p>");
		document.write('<center><object width=50% height=50% border=0 name="movie" classid="clsid:D27CDB6E-AE6D-11cf-96B8-444553540000" codebase="http://download.macromedia.com/pub/shockwave/cabs/flash/swflash.cab#version=9,0,28,0" width="720" height="576">\
				<param name="movie" value="' + tzerfile + '">\
				<param name="quality" value="high">\
				<param name="scale" value="showall">\
				<param name="wmode" value="transparent">\
				<embed src="' + tzerfile + '" quality="high" pluginspage="http://www.adobe.com/shockwave/download/download.cgi?P1_Prod_Version=ShockwaveFlash" wmode="transparent" type="application/x-shockwave-flash" width="720" height="576">\
				</embed></object></center></div>');
	}
	else if (text.indexOf('id=flash_4')!= -1)
	{
		tzerfile = '!tools\\tZersFiles\\mailru\\devochka.swf';
		document.write("<img src='!tools/tZersFiles/mailru/PNG/mult_girl.png'/><p><i>Хны...!</i><p>");
		document.write('<center><object width=50% height=50% border=0 name="movie" classid="clsid:D27CDB6E-AE6D-11cf-96B8-444553540000" codebase="http://download.macromedia.com/pub/shockwave/cabs/flash/swflash.cab#version=9,0,28,0" width="720" height="576">\
				<param name="movie" value="' + tzerfile + '">\
				<param name="quality" value="high">\
				<param name="scale" value="showall">\
				<param name="wmode" value="transparent">\
				<embed src="' + tzerfile + '" quality="high" pluginspage="http://www.adobe.com/shockwave/download/download.cgi?P1_Prod_Version=ShockwaveFlash" wmode="transparent" type="application/x-shockwave-flash" width="720" height="576">\
				</embed></object></center></div>');
	}
	else if (text.indexOf('id=flash_3')!= -1)
	{
		tzerfile = '!tools\\tZersFiles\\mailru\\sobaka.swf';
		document.write("<img src='!tools/tZersFiles/mailru/PNG/mult_dog.png'/><p><i>Хочу к тебе!</i><p>");
		document.write('<center><object width=50% height=50% border=0 name="movie" classid="clsid:D27CDB6E-AE6D-11cf-96B8-444553540000" codebase="http://download.macromedia.com/pub/shockwave/cabs/flash/swflash.cab#version=9,0,28,0" width="720" height="576">\
				<param name="movie" value="' + tzerfile + '">\
				<param name="quality" value="high">\
				<param name="scale" value="showall">\
				<param name="wmode" value="transparent">\
				<embed src="' + tzerfile + '" quality="high" pluginspage="http://www.adobe.com/shockwave/download/download.cgi?P1_Prod_Version=ShockwaveFlash" wmode="transparent" type="application/x-shockwave-flash" width="720" height="576">\
				</embed></object></center></div>');
	}
	else if (text.indexOf('id=flash_2')!= -1)
	{
		tzerfile = '!tools\\tZersFiles\\mailru\\zadolbal.swf';
		document.write("<img src='!tools/tZersFiles/mailru/PNG/mult_bored.png'/><p><i>Задолбал!</i><p>");
		document.write('<center><object width=50% height=50% border=0 name="movie" classid="clsid:D27CDB6E-AE6D-11cf-96B8-444553540000" codebase="http://download.macromedia.com/pub/shockwave/cabs/flash/swflash.cab#version=9,0,28,0" width="720" height="576">\
				<param name="movie" value="' + tzerfile + '">\
				<param name="quality" value="high">\
				<param name="scale" value="showall">\
				<param name="wmode" value="transparent">\
				<embed src="' + tzerfile + '" quality="high" pluginspage="http://www.adobe.com/shockwave/download/download.cgi?P1_Prod_Version=ShockwaveFlash" wmode="transparent" type="application/x-shockwave-flash" width="720" height="576">\
				</embed></object></center></div>');
	}
	else if (text.indexOf('id=flash_1')!= -1)
	{
		tzerfile = '!tools\\tZersFiles\\mailru\\beer.swf';
		document.write("<img src='!tools/tZersFiles/mailru/PNG/mult_beer.png'/><p><i>Пивка?;)</i><p>");		
		document.write('<center><object width=50% height=50% border=0 name="movie" classid="clsid:D27CDB6E-AE6D-11cf-96B8-444553540000" codebase="http://download.macromedia.com/pub/shockwave/cabs/flash/swflash.cab#version=9,0,28,0" width="720" height="576">\
				<param name="movie" value="' + tzerfile + '">\
				<param name="quality" value="high">\
				<param name="scale" value="showall">\
				<param name="wmode" value="transparent">\
				<embed src="' + tzerfile + '" quality="high" pluginspage="http://www.adobe.com/shockwave/download/download.cgi?P1_Prod_Version=ShockwaveFlash" wmode="transparent" type="application/x-shockwave-flash" width="720" height="576">\
				</embed></object></center></div>');
	}
	else if (text.indexOf('id=flash_0')!= -1)
	{
		tzerfile = '!tools\\tZersFiles\\mailru\\guby.swf';
		document.write("<img src='!tools/tZersFiles/mailru/PNG/mult_kiss.png'/><p><i>Поцелуй</i><p>");
		document.write('<center><object width=50% height=50% border=0 name="movie" classid="clsid:D27CDB6E-AE6D-11cf-96B8-444553540000" codebase="http://download.macromedia.com/pub/shockwave/cabs/flash/swflash.cab#version=9,0,28,0" width="720" height="576">\
				<param name="movie" value="' + tzerfile + '">\
				<param name="quality" value="high">\
				<param name="scale" value="showall">\
				<param name="wmode" value="transparent">\
				<embed src="' + tzerfile + '" quality="high" pluginspage="http://www.adobe.com/shockwave/download/download.cgi?P1_Prod_Version=ShockwaveFlash" wmode="transparent" type="application/x-shockwave-flash" width="720" height="576">\
				</embed></object></center></div>');
	}
 }
}