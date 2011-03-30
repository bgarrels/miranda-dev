$( function() {	
	
	function safeHTML(sHTML)
	{
		if ( !sHTML ) return '';
		return $('<div />').text(sHTML).html() ;
	}

	// update relative timestamps if required
	setInterval( function() {
		$('.msg-ts').each( function() {
			var ts = $(this).attr('rel');
			if ( ts ) $(this).html( fmtTimestamp(parseInt(ts)) );
		});
	}, 5000);
	
	function fmtTimestamp(ts)
	{
		var t = new Date(ts).getTime() ;
		var dnow = new Date();
		var now = dnow.getTime();
		var diff = Math.round( ( now - t ) / 1000 );
		var str = '';
	
		switch ( true ) 
		{
			case ( diff == 0 ):
				diff = '';
				str = '';
				break;
				
			case (diff < 60):
				str = 'second';
				break;
				
			case (diff < 60*59):
				diff = Math.round(diff / 60);
				str = 'minute';
				break;
				
			case (diff < 60*60*24):
				diff = Math.round(diff / 60 / 60);
				str = 'hour'
				break;
				
			case ( diff < 60*60*24*30 ):
				diff = Math.round(diff / 60 / 60 / 24);
				str = 'day';
				break;
			
			default:
				diff = new Date(ts).toUTCString();
				str = '';
				break;
		}

		str = diff + ' ' + str + ( diff > 0 && diff != 1 ? 's' : '' ) + ( diff > 0 ? ' ago' : '' );
		return '<span class="msg-ts" rel="'+ts+'">'+str+'</span>';
	}
	
	function setHeight() {
		var headerH = document.getElementById('cltoolbartop').offsetHeight,
			footerH = document.getElementById('cltoolbar').offsetHeight,
			wrapperH = window.innerHeight - headerH - footerH;
		document.getElementById('wrapper').style.height = wrapperH + 1 + 'px';
	}

	var iScr = new iScroll('wrapper', { 
		//checkDOMChanges: false ,
		hScrollbar: false ,
	});
	
	// in ms
	var _lastEvent;
	
	function updateIdle()
	{
		_lastEvent = new Date().getTime();
	}
	function IdleUser()
	{
		return new Date().getTime() - _lastEvent > 1000 * 60;
	}
	
	/* different browsers support different codecs, Safari doesn't support Ogg and Firefox
	doesn't support MP3, all support WAV but that is too big, Safari has to have its own
	special version of WAV too
	*/
	function playSound(fn, hContact)
	{
		var aud = $('<audio />').get(0);
		$(aud)
			.attr('src', '../sounds/' + fn)
			.bind( "loadstart", function() {
				aud.play();
			}) ;
			
		aud.load();
	}
	
	updateIdle();

	window.addEventListener('onorientationchange' in window ? 'orientationchange' : 'resize', setHeight, false);
	document.addEventListener('touchmove', 
		function(e) { 
			e.preventDefault(); 
		} , false); 
		
	$('body').bind('mousemove touchmove click', function(event) {
		updateIdle();
	});
	
	mUI = {};
	getchatui(mUI);
	
	function updateScroll()
	{
		setTimeout(function () { iScr.refresh(); }, 50) ;
	}
	
	function scrollToBottom()
	{
		var Obj = $('#sview').children(':last-child').get(0);
		if ( Obj.offsetTop > $('#wrapper').height() )
		{
			// scroll to the bottom if not there
			iScr.scrollToElement(Obj);
		}	
	}

	function mkChatEntry(args)
	{
		var sHTML;
		
		switch ( args.type )
		{
			case 'msg':
			{
				sHTML = mUI.onMsgIncoming(args);
				break;
			}
			case 'selfmsg':
			{
				sHTML = mUI.onMsgOutgoing(args);
				break;
			}
			case 'status':
			{
				sHTML = mUI.onMsgStatus(args);
				break;
			}
			default:
			{
				sHTML = "<p>event (" + args.type + ") unhandled.</p>";
			}
		}
		var oMsg = $(sHTML);
		$('.msg-text', oMsg).click( function() {
		});
		$(oMsg).appendTo("#msg");
		return oMsg;
	}
	
	function getMyAvatar(proto)
	{
		var o = $('#_0 .cl-self-avatar-' + proto);
		if ( !o.length )
			o = $('#_0 .cl-self-avatar-');
		return o;
	}
	
	function OnContactClick(cData, contactDiv)
	{
		$('#cltitle').text(cData.nickname);
		
		changeView('#msg', function() {
			
			$('#msg')
				.html('')
				.data("activeContact", cData.hContact)
			;
			
			$('#wrapper')
				.addClass('ajaxload')
			;
			
			startUnreadMsgRequest(0, true);
			
			function startMarkMsgRequest(lasteid)
			{
				a = {
					cid: cData.hContact ,
					eid: lasteid ,
					chain: 1
				};
				$.get('/api/msg/markread', a, function(xml, status) {
				});
			}
			
			function startUnreadMsgRequest(lasteid, firstload)
			{
				getargs = {
					cid: cData.hContact ,
					preview: '1' ,  
					self: firstload ? 1 : 0
				};
				$.get('/api/msg/unread', getargs, function(xml, status) {
					
					$('#wrapper')
						.removeClass('ajaxload')
					;
					
					var contactAvatar = $('.cl-contact-icon-avatar', contactDiv).attr('src');
					var selfAvatar = getMyAvatar(cData.proto).attr('src');
				
					var args = {
						message: "" ,
						ts: ""
					};
					
					var eid = 0;
					var oMsg;
					
					var item = $(xml).find("item[cid='" + cData.hContact + "']");
					
					$(item).find("event").each( function() {
						eid = $(this).attr('eid');
						if ( eid > lasteid ) 
						{ 
							args.message = safeHTML( $(this).text() ) ;
							args.unread = $(this).attr('unread') ? true : false;
							args.ts = fmtTimestamp( $(this).attr('ts') * 1000 );
							args.type = safeHTML( $(this).attr('type') );
							args.avatar = ( args.type == "msg" ) ? contactAvatar : selfAvatar;
	
							oMsg = mkChatEntry(args);
							updateScroll();
						}
					});
					
					if ( oMsg ) 
					{ 
						updateScroll();
						setTimeout( function() {
							scrollToBottom();
						}, 750);
					}

					if ( firstload )
						$('#sendmsg').show();
					
					setTimeout( function() {
						if ( !$('#msg').is(':visible') )
							return;
						// mark the the last message (and the ones before) as unread
						if ( eid ) startMarkMsgRequest(eid) ;
						startUnreadMsgRequest(eid, false);
					} , 5000 );
				});
			};
		
		});
	}
	
	function OnSelfMessage(hContact, text)
	{
		args = { cid: hContact, msg: text };
		$.get( '/api/msg/send', args , 
			function(xml, status) {
				
			} 
		, "xml");
	}
	
	function mkSendBox()
	{
		function sendTyping(start)
		{
			m = { cid: $('#msg').data("activeContact"), stop: start ? 0 : 1 };
			$.get('/api/notify/typing', m, function(xml,status) {
				});
		}
				
		args = {};
		args.message = "<input text='text' id='cl-send-area' />";
		args.unread = false;
		args.ts = "";
		args.hideavatar = true;

		var sHTML = mUI.onMsgOutgoing(args);
		var sendArea = $(sHTML);
		$(sendArea).appendTo('#sendmsg');
		var typingTimeout;
		$('#cl-send-area')
			.bind('keyup', function(event) {
				
				if ( typingTimeout )
				{
					clearTimeout(typingTimeout); 
					typingTimeout = null;
				}
				else
				{
					sendTyping(true);   	// started typing
				}
							
				typingTimeout = setTimeout( function() 
				{ 
					typingTimeout = null; 	// finished typing
					sendTyping(false);
				} , 5000);
			})
			.change( function(event) {
						
			// shouldnt happen
			if ( !$('#msg').is(':visible') )
				return;
			
			if ( typingTimeout ) 
			{ 
				clearTimeout(typingTimeout); typingTimeout = null;
			}
			
			var hContact = $('#msg').data("activeContact");
			var txtmsg = jQuery.trim( $(this).val() );
			if ( txtmsg.length < 1 )
				return;

			OnSelfMessage( hContact, txtmsg );
			
			cData = $('#_' + hContact).data("cData");
			// inject
			args.message = safeHTML(txtmsg);
			args.ts = fmtTimestamp( new Date().getTime() );
			args.avatar = getMyAvatar(cData.proto).attr('src');
					
			var sHTML = mUI.onMsgOutgoing(args);
			$(sHTML).appendTo('#msg');
			
			updateScroll();
			setTimeout( function() { 
				scrollToBottom();  
			}, 500);
			
			// reset content
			$(this).val("");
			event.preventDefault();
		});
	}
	
	function mkContactList()
	{
		var clxml

		if ( !clxml )
		{
			$.get('/api/clist', function(xml, status) {
					clxml = xml;
					render(xml);
					checkUnread();
					checkAvatars();
					setInterval(checkContacts, 1000 * 10);
					setInterval(checkUnread, 1000 * 5);
					setInterval(checkAvatars, 1000 * 60 * 2);
				}, "xml");
		}
		else
		{
			render(clxml);
		}
		
		var tm;
		function _clSort(reason)
		{
			$('#cl').children().sort( function(a, b) {
				var x = $(a).data("cData");
				var y = $(b).data("cData");
				
				if ( !x || !y )
					return x ? 1 : -1;
				
				function compareUnreads()
				{
					return y.unread - x.unread;
				}
				
				function compareNicknames()
				{
					if ( x.unick == y.unick )
						return 0;
					return x.unick > y.unick ? 1 : -1;
				}
				
				function compareStatuses()
				{
					if ( x.statusw == y.statusw ) 
						return 0;
					return x.statusw < y.statusw ? 1 : -1;
				}
				
				var r;
				
				r = compareUnreads();
				if ( r != 0 )
					return r;
				
				r = compareStatuses();
				if ( r != 0 )
					return r;
				
				r = compareNicknames();
				return r;
				
			}).prependTo('#cl');
			tm = null;
		}
		
		function clSort(reason)
		{
			if ( tm ) 
			{
				clearTimeout(tm); tm = null;
			}
			tm = setTimeout( function() { _clSort(reason); }, 250);
		}
		
		var i = 1;
		
		function checkContacts()
		{
			$.get( '/api/clist', function(xml, status) {
				
				var needsort = false;
				var hContact;
				
				$(xml).find("item").each( function() {

					var newData = {
						hContact: safeHTML( $(this).attr("cid") ),
						nickname: safeHTML( $(this).text() ),
						status: safeHTML( $(this).attr("status") ),
						statusw: safeHTML( $(this).attr("statusw") ),
						proto: safeHTML( $(this).attr("proto") ),
					};
					
					// faster compare during sort
					newData.unick = newData.nickname.toLowerCase();
										
					var ctx = $('#_' + newData.hContact);
					cData = $(ctx).data("cData");
										
					// update status icon and label 
					if ( cData.status != newData.status )
					{
						$('.cl-contact-icon', ctx)
							.removeClass('cl-icon-' + cData.proto + '-' + cData.status )
							.addClass('cl-icon-' + newData.proto + '-' + newData.status )
						;
						$('.cl-contact-text', ctx)
							.removeClass('cl-contact-text-' + cData.status)
							.addClass('cl-contact-text-' + newData.status)
						;
						$('.cl-contact-status',ctx)
							.text( newData.status )
							.attr("weight", newData.statusw )
						;
						
						// update any chat window
						var oMsg = $('#msg');
						var hContact = $(oMsg).data("activeContact");
						if ( $(oMsg).is(':visible') && hContact == newData.hContact )
						{
							args.type = "status";
							args.message = newData.nickname + " is " + newData.status;
							
							mkChatEntry(args);
						}
						
						needsort = true;
					}
					
					// update nickname
					if ( cData.nickname != newData.nickname )
					{
						$('.cl-contact-text',ctx).text( newData.nickname );
						needsort = true;
					}
					
					// store everything as needed
					for ( f in newData ) cData[f] = newData[f];
					
					$(ctx).data("cData", cData);
					
				});
				
				if ( needsort ) clSort("contact");
				
			}, "xml");
		}
		
		function checkAvatars()
		{
			$.get('/api/contact/avatar', function(xml, status) {
				$(xml).find("item").each( function() {
					var cData = {
						hContact: safeHTML( $(this).attr("cid") ),
						proto: safeHTML( $(this).attr("proto") ) // can be empty
					};
										
					if ( cData.hContact > 0 )
					{
						var ico = $('#_' + cData.hContact + ' .cl-contact-icon-avatar');
						$(ico)
						.attr('src', '/api/contact/avatar?cid=' + cData.hContact)
						.show()
						;
					}
					else
					{
						var self = getMyAvatar(cData.proto);

						if ( !self.length )
						{
							self = $('<img>');
							$(self)
								.addClass('cl-self-avatar-' + cData.proto);
							$(self).appendTo('#_0');
						}
						
						$(self).attr('src', '/api/contact/avatar?cid=0&proto=' + cData.proto);

					}
				}); //each
			}, "xml");
		}
		
		function checkUnread()
		{
			$.get('/api/msg/unread', function(xml, status) {
				
				var needsort = false;
				var needsound = false;
				$('.cl-contact-unread:visible').addClass('inactivebadge');
				
				var hActiveContact = $('#msg:visible').data("activeContact");
				var otherEvents = 0;
				
				$(xml).find("item").each( function() {
					var newData = {
						hContact: safeHTML($(this).attr("cid")) ,
						unread: safeHTML($(this).find("unread").text())
					};
					var ur = $('#_' + newData.hContact + ' .cl-contact-unread');
					var ctx = $(ur).parent();
					var cData = $(ctx).data("cData");
					
					if ( newData.unread != cData.unread )
					{
						needsort = true ;
					}
					
					if ( newData.unread > cData.unread && ( hActiveContact != newData.hContact || idleUser() ) )
					{
						needsound = true;
					}
										
					$(ur)
						.removeClass('inactivebadge')
						.text( newData.unread )
						.show()
					;
				
					for ( f in newData ) cData[f] = newData[f];
					$(ctx).data("cData", cData);
					
					if ( hActiveContact && hActiveContact != newData.hContact || !hActiveContact && !$('#cl').is(':visible')  )
					{
						otherEvents += parseInt(newData.unread);
					}
				}); //each
				
				if ( otherEvents > 0 )
				{
					$('#cl-other-events')
						.text(otherEvents)
						.show("fade")
					;
				}
				
				if ( needsound )
				{
					playSound('message.mp3', 0);
				}
				
				// any remaining stuff should be reset
				var dead = $('.inactivebadge');
				if ( dead.length ) needsort = true;
				
				$(dead)
					.hide()
					.removeClass('inactivebadge')
					.text(0)
				; 
				
				if ( needsort ) clSort("unread");
				
			}, "xml");
		}
				
		function render(xml)
		{
			$(xml).find("item").each( function() 
			{
				var cData = {
					hContact: safeHTML($(this).attr("cid")) ,
					nickname: safeHTML($(this).text()) ,
					status: safeHTML($(this).attr("status")) ,
					statusw: safeHTML($(this).attr("statusw")) ,
					proto: safeHTML($(this).attr("proto")),
					unread: 0
				};
				
				cData.unick = cData.nickname.toLowerCase();
								
				var div = $("<div />")
					.attr('id', '_' + cData.hContact) 
					.addClass('cl-contact')
					.data("cData", cData)
					;
					
				var icon = $('<img>')
					.hide()
					.addClass('cl-contact-icon-avatar')
				;
				
				var proto = $('<img />')
					.addClass('cl-contact-icon cl-icon-' + cData.proto + '-' + cData.status)
					.attr('src', 'images/1x1.png')
					.click( function() {
						OnContactClick(cData, div);
					})
				;
							
				var ctext = $('<span>' + cData.nickname + '</span>')
					.addClass('cl-contact-text cl-contact-text-' + cData.status)
					.click( function() {
						OnContactClick(cData, div);
					})
				;
					
				var unread = $('<span>' + '0' + '</span>')
					.addClass('cl-contact-unread')
					.hide()
					.click( function() {
						OnContactClick(cData, div);
					})
				;
					
				var status = $('<span>' + cData.status + '</span>')
					.addClass('cl-contact-status')
					.attr('weight', cData.statusw)
					.hide()
					;
										
				$(div)
					.append(proto)
					.append(ctext)
					.append(unread)
					.append(status)
					
					.append(icon)
						.appendTo("#cl")
				;
		
			}); //each item
			
			// add a hidden contact to hold self information 
			var div = $("<div />")
				.attr('id', '_0') 
				.addClass('cl-contact-self')
				.hide()
			;
			$(div).appendTo("#cl");
			
			// sort by name
			clSort("init");
			
			setHeight();
			updateScroll();
		}
	}
	
	$('.acc-status-combo').live( "change", function(event) {
		a = {
			acid: $(this).data("acid") ,
			setstatus: $(this).val() ,
		};
		$.get('/api/account', a, function(xml, status) {			
		})
	});
	
	function OnAccountView()
	{
		$('#wrapper').addClass('ajaxload pinbackground');
		$('#acc').children().remove();
		
		function statusBox(status, statusmask, acid)
		{
			
			var PF2_OFFLINE = 0;
			var PF2_ONLINE        =0x00000001;   //an unadorned online mode
			var PF2_INVISIBLE     =0x00000002;
			var PF2_SHORTAWAY     =0x00000004;   //Away on ICQ, BRB on MSN
			var PF2_LONGAWAY      =0x00000008;   //NA on ICQ, Away on MSN
			var PF2_LIGHTDND      =0x00000010;   //Occupied on ICQ, Busy on MSN
			var PF2_HEAVYDND      =0x00000020;   //DND on ICQ
			var PF2_FREECHAT      =0x00000040;
			var PF2_OUTTOLUNCH    =0x00000080;
			var PF2_ONTHEPHONE    =0x00000100;
			
			var statusMap = [PF2_OFFLINE, PF2_ONLINE, PF2_INVISIBLE, PF2_SHORTAWAY, PF2_LONGAWAY, PF2_LIGHTDND, 
				PF2_HEAVYDND, PF2_FREECHAT, PF2_OUTTOLUNCH, PF2_ONTHEPHONE];
			var statusMapOpt = [ 'offline', 'online', 'invisible', 'away', 'na', 'occupied', 'dnd', 
				'freechat', 'outtolunch', 'onthephone'];
			var statusMapDesc = [ 'Offline', 'Online', 'Invisible', 'Away', 'N/A', 'Occupied', 'DND',
				'Free for chat', 'Out to lunch', 'On the phone'];
			
			var sHTML = '<select class="acc-status-combo" data-status="' + status + '" data-acid="' + acid +'">';
			sHTML += '<option ' + ( status=='offline' ? 'selected':'' ) + 'value="offline">Offline</option>';

			$(statusMap).each( function(i) {
				if ( this & statusmask )
				{
					var opt = statusMapOpt[i];
					sHTML += '<option ' + ( status == opt ? 'selected':'' )  + ' value="' + opt + '">' + statusMapDesc[i] + '</option>';
				}			
			});
			
			return sHTML + '</select>';
		}
			
		$.get( '/api/account', function(xml, status) {
			
			$(xml).find('item').each( function() {
			
				var acid = $(this).attr('acid');
				var div = $('<div />')
					.addClass('acc-win');
				var l = $('<label />');
				$(l)
					.text( acid.toLowerCase() )
					.appendTo(div)
				; 
				
				var st = $(this).attr('status');
				var stm = $(this).attr('status_mask');
				var sCombo = statusBox(st, stm, acid);
				
				sHTML = '<table class="acc-table">';
				sHTML += '<thead></thead>';
				sHTML += '<tbody>';
				sHTML += '<tr><td class="acc-table-field">status</td> <td class="acc-table-fieldvalue">' 
					+ sCombo + '</td></tr>';
				sHTML += '</tbody></table>';
				
				$(div).append(sHTML).appendTo('#acc');
			});

			updateScroll(); 
			$('#wrapper').removeClass('ajaxload');
		});
	}
	
	mkContactList();
	mkSendBox();
	
	var activeDiv = '#cl';
	
	function onViewStart(view)
	{
		switch ( view )
		{
			case '#cl':
				$('#cl-other-events').hide("fade");
			case '#acc':
				if ( view == '#acc' ) OnAccountView() ;
				$('#cltitle').text('Miranda IM');
				$('#sendmsg').hide();
			break;
				
			case '#msg':
			{
				
				break;
			}
		}
	}
	
	function onViewStop(view)
	{
		switch ( view ) 
		{
			case '#acc':
				$('#wrapper').removeClass('pinbackground');
				break;
		}
	}
	
	function changeView(newView, OnBeforeChange)
	{
		if ( activeDiv == newView )
			return false;
		
		if ( OnBeforeChange )
			OnBeforeChange();
		
		$(activeDiv).hide( 0, function() {
			onViewStop(activeDiv);
			onViewStart(newView);
			$(newView).show();
			activeDiv = newView;
			
			updateScroll();
			
			// show the active LI based on the related DIV
			$('#cltoolbar li.active').removeClass('active');
			$('#cltoolbar li[rel="' + activeDiv + '"]').addClass('active');
				
		});
		
		return true;
	}
	
	// titlebar click
	$('#cltoolbartop').click( function() {
		iScr.scrollTo(0, 0, '150ms');
	});

	// menu tool buttons
	$('#cltoolbar > ul > li').click( function() {
		changeView( $(this).attr('rel') );
	});
	
});
