//Settings
MaxSimGroups = 10; //Maximum simultaneously groups

//Ids
MessageIn = 1;
hMessageIn = 2;
MessageInGroupStart = 3;
MessageInGroupInner = 4;
MessageInGroupEnd = 5;
hMessageInGroupStart = 6;
hMessageInGroupInner = 7;
hMessageInGroupEnd = 8;
MessageOut = 9;
hMessageOut = 10;
MessageOutGroupStart = 11;
MessageOutGroupInner = 12;
MessageOutGroupEnd = 13;
hMessageOutGroupStart = 14;
hMessageOutGroupInner = 15;
hMessageOutGroupEnd = 16;
Status = 17;
hStatus = 18;
URL = 19;
hURL = 20;
File = 21;
hFile = 22;

names = new Array();
names[MessageIn] = 'MessageIn';
names[hMessageIn] = 'hMessageIn';
names[MessageInGroupStart] = 'MessageInGroupStart';
names[MessageInGroupInner] = 'MessageInGroupInner';
names[MessageInGroupEnd] = 'MessageInGroupEnd';
names[hMessageInGroupStart] = 'hMessageInGroupStart';
names[hMessageInGroupInner] = 'hMessageInGroupInner';
names[hMessageInGroupEnd] = 'hMessageInGroupEnd';
names[MessageOut] = 'MessageOut';
names[hMessageOut] = 'hMessageOut';
names[MessageOutGroupStart] = 'MessageOutGroupStart';
names[MessageOutGroupInner] = 'MessageOutGroupInner';
names[MessageOutGroupEnd] = 'MessageOutGroupEnd';
names[hMessageOutGroupStart] = 'hMessageOutGroupStart';
names[hMessageOutGroupInner] = 'hMessageOutGroupInner';
names[hMessageOutGroupEnd] = 'hMessageOutGroupEnd';
names[Status] = 'Status';
names[hStatus] = 'hStatus';
names[URL] = 'URL';
names[hURL] = 'hURL';
names[File] = 'File';
names[hFile] = 'hFile';

function AfterEachGroup(group) {
	if (group.length>MaxSimGroups) {
		group.addClass('finished');
		return false;
	}
	return true;
}

function AfterMessageGroupStart(group) {
	head = group.find('.head');
	headname = group.find('.head .name');
	headtime = group.find('.head .time');
	text = group.find('.text');
	
	headname.css('opacity', '0');
	headname.css('margin-left', '60px');
	headname.animate({'opacity': 1, 'marginLeft': '0px'}, 900+Math.floor(Math.random()*900),
		function() {
			if(jQuery.browser.msie) {
				$(this).get(0).style.removeAttribute('filter');
			}
		}
	);
	
	headtime.css('opacity', '0');
	headtime.css('margin-right', '60px');
	headtime.animate({'opacity': 1, 'marginRight': '0px'}, 900+Math.floor(Math.random()*900),
		function() {
			if(jQuery.browser.msie) {
				$(this).get(0).style.removeAttribute('filter');
			}
		}
	);
	
	text.css('opacity', '0');
	text.css('margin-left', '60px');
	text.animate({'opacity': 1, 'marginLeft': '0px'}, 900+Math.floor(Math.random()*300),
		function() {
			if(jQuery.browser.msie) {
				$(this).get(0).style.removeAttribute('filter');
			}
		}
	);
}

function AfterMessageGroupInner(group, groupname) {
	gtime = group.find('.gtime');
	text = group.find('.text');
	
	gtime.css('opacity', '0');
	gtime.css('margin-right', '60px');
	gtime.animate({'opacity': 1, 'marginRight': '0px'}, 900+Math.floor(Math.random()*900),
		function() {
			if(jQuery.browser.msie) {
				$(this).get(0).style.removeAttribute('filter');
			}
		}
	);
	
	text.css('opacity', '0');
	text.css('margin-left', '60px');
	text.animate({'opacity': 1, 'marginLeft': '0px'}, 900+Math.floor(Math.random()*300),
		function() {
			if(jQuery.browser.msie) {
				$(this).get(0).style.removeAttribute('filter');
			}
		}
	);
}

function ProcessGroup( groupid ) {
	ndx = document.body.childNodes.length;
	ndx--;
	elm = document.body.childNodes[ndx];
	
	while (elm.type!=undefined) {
		ndx -= 2;
		elm = document.body.childNodes[ndx];
	}
	
	group = $(elm);
	groupname = names[groupid];
	if (groupname!=undefined) {
		if (!group.hasClass(groupname)) {
			group = group.find('.'+groupname+':not(.__finished)');
		}
	}
	switch (groupid) {
		case MessageInGroupStart: 
			if (!AfterEachGroup(group)) { return; }
			AfterMessageGroupStart(group);
			break;
		case MessageInGroupInner:
			if (!AfterEachGroup(group)) { return; }
			AfterMessageGroupInner(group);
			break;
		case MessageOutGroupStart:
			if (!AfterEachGroup(group)) { return; }
			AfterMessageGroupStart(group);
			break;
		case MessageOutGroupInner:
			if (!AfterEachGroup(group)) { return; }
			AfterMessageGroupInner(group);
			break;
		default:
			return;
	}
	
	group.addClass('__finished');
}