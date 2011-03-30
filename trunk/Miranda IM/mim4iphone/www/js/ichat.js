
function getchatui(p)
{
	
	function mkTable(message, align, cls)
	{
		var sHTML = '<table cellpadding="0" cellspacing="0" align="'+align+'">' +
'<tr><td>'+
	'<td class="minsize">'+
	'<div class="'+cls+'center">'+
	'<div class="'+cls+'top">'+
	'<div class="'+cls+'bottom">'+
	'<div class="'+cls+'left">'+
	'<div class="'+cls+'right">'+
	'<div class="'+cls+'lefttop">'+
	'<div class="'+cls+'righttop">'+
	'<div class="'+cls+'rightbottom">'+
	'<div class="'+cls+'leftbottom">'+
	'<div class="textmargin msg-text msg-text-'+cls+'">' + message + '</div></div></div></div></div></div></div></div></div></div></td>'+
'</td></tr>'+
'</table>' ;
		return sHTML;
	}
	
	p.onMsgIncoming = function(args)
	{
		
		var imgsrc = args.avatar ? args.avatar : 'images/noavatar.png';
		var cls = 'in';
		var dateStr = args.ts ? args.ts : '';
		
		var sHTML = '<table cellpadding="0" cellspacing="0">' +
			'<tr><td width="100%" align="right">' + mkTable(args.message, "right", cls) +

			'</td><td>&nbsp;</td><td width="32" style="vertical-align: bottom;">'+
			'<img class="msg-avatar-in" src="' + imgsrc + '" width="32" /></td></tr>  </table>'+

			'<table cellpadding="0" cellspacing="0" class="spacer"> <tr><td align="right" class="msg-date">'
				+dateStr+'</td></tr>  </table>' ;
		return sHTML;
	}
	
	p.onMsgOutgoing = function(args)
	{
		
		var imgsrc = args.avatar ? args.avatar : 'images/noavatar.png';
		var dateStr = args.ts ? args.ts : '';
		var imgtag = '<img class="msg-avatar-out" src="'+ imgsrc +'" width="32" />';
		
		if ( args.hideavatar ) imgtag = '';
		
		var sHTML = '<table cellpadding="0" cellspacing="0">' +
'<tr><td width="32" style="vertical-align: bottom;">'+
		imgtag + '</td><td>&nbsp;</td><td width="100%">'
			
			+ mkTable(args.message, "left", "out") + '</td></tr></table>'+

			'<table cellpadding="0" cellspacing="0" class="spacer"> <tr><td align="left" class="msg-date">'
				+ dateStr + '</td></tr> </table>';

		return sHTML;
	}
	
	p.onMsgStatus = function(args)
	{
		var sHTML = '<table cellpadding="0" cellspacing="0">' +
			'<tr><td width="100%" align="right">' + mkTable(args.message, "center", "status") +

			'</td><td>&nbsp;</td> </tr></table>' ;
		return sHTML;
	}
}
