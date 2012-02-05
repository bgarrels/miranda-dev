#include "stdhdr.h"

static	void	ProcessDroppedItems	( char **ppDroppedItems, int nCount, char **ppFiles );
static	int		CountDroppedFiles	( char **ppDroppedItems, int nCount  );
static	BOOL	OnDropFiles			( HDROP hDrop, ThumbInfo *pThumb );

HRESULT STDMETHODCALLTYPE CDropTarget::QueryInterface(REFIID riid,LPVOID *ppvObj)
{
	if ( IsEqualIID( riid, IID_IDropTarget ) ) 
	{
		*ppvObj = this;
		this->AddRef();
		return S_OK;
	}
	
	*ppvObj=NULL;

	return ( E_NOINTERFACE );
}

ULONG STDMETHODCALLTYPE CDropTarget::AddRef( )
{
	return ++this->refCount;
}

ULONG STDMETHODCALLTYPE CDropTarget::Release( )
{
	int res = --this->refCount;
	if (!res) delete this;
	return res;
}


HRESULT STDMETHODCALLTYPE CDropTarget::DragOver( DWORD fKeyState, POINTL pt, DWORD *pdwEffect )
{
	*pdwEffect = 0;
	
	if( hwndCurDrag == NULL ) 
	{
		*pdwEffect = DROPEFFECT_NONE;
	}
	else
	{
		*pdwEffect |= DROPEFFECT_COPY;
	}
	return S_OK;
}


HRESULT STDMETHODCALLTYPE CDropTarget::DragEnter( IDataObject *pData, DWORD fKeyState, POINTL pt, DWORD *pdwEffect)
{
	HWND		hwnd	= NULL;
	POINT		shortPt;
	FORMATETC	feFile	= { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
	FORMATETC	feText	= { CF_TEXT, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
	ThumbInfo *pThumb;

	if ( S_OK == pData->QueryGetData( &feFile ) ||
		 S_OK == pData->QueryGetData( &feText ) )
	{
		shortPt.x	= pt.x; 
		shortPt.y	= pt.y;
		
		hwnd = WindowFromPoint( shortPt );
		
		if ( pThumb = thumbList.FindThumb( hwnd ) )
		{
			hwndCurDrag = hwnd;
			pThumb->ThumbSelect( TRUE );
		}
	}

	return DragOver( fKeyState, pt, pdwEffect);
}


HRESULT STDMETHODCALLTYPE CDropTarget::DragLeave( )
{
	ThumbInfo *pThumb = thumbList.FindThumb( hwndCurDrag );

	if ( NULL != pThumb )
	{
		pThumb->ThumbDeselect( TRUE );
	}

	hwndCurDrag = NULL;
	
	return S_OK;
}


HRESULT STDMETHODCALLTYPE CDropTarget::Drop( IDataObject *pData,DWORD fKeyState,POINTL pt,DWORD *pdwEffect)
{
	FORMATETC	fe		= { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
	STGMEDIUM	stg;
	HDROP		hDrop		= NULL;
	ThumbInfo	*pThumb		= NULL;
	char		*pText		= NULL;
	BOOL		bFormatText = FALSE;

	*pdwEffect = DROPEFFECT_NONE;
	

	if( hwndCurDrag == NULL ) return( S_OK );

	pThumb	= (ThumbInfo*)GetWindowLong( hwndCurDrag, GWLP_USERDATA );
	if ( pThumb == NULL ) return( S_OK );

	if ( S_OK != pData->GetData( &fe,&stg ) )
	{
		fe.cfFormat = CF_TEXT;

		if ( S_OK != pData->GetData( &fe,&stg ) )
		{
			return( S_OK );
		}
		else
		{
			bFormatText = TRUE;
		}
	}
		
	

	if ( !bFormatText )
	{
		hDrop	= (HDROP)stg.hGlobal;
		
		if ( hDrop != NULL )
		{
			OnDropFiles( hDrop, pThumb );
		}
	}
	else
	{
		pText = (char*)GlobalLock( stg.hGlobal );

		if ( pText != NULL  )
		{
			SendMsgDialog( hwndCurDrag, pText );
			GlobalUnlock( stg.hGlobal );
		}
	}

	if( stg.pUnkForRelease != NULL )
	{
		stg.pUnkForRelease->Release( );
	}
	else 
	{
		GlobalFree( stg.hGlobal );
	}

	DragLeave( );
	
	return S_OK;
}

///////////////////////////////////////////////////////
// Send files processing

BOOL OnDropFiles( HDROP hDrop, ThumbInfo *pThumb )
{
	BOOL	bSuccess			= FALSE;
	UINT	nFilesCount			= 0;
	UINT	iItem				= 0;
	char	**ppFiles			= NULL;	
	char	**ppDroppedItems	= NULL;
	UINT	nDroppedItemsCount	= 0;
	char	szFilename[ MAX_PATH ];
	

	nDroppedItemsCount = DragQueryFile( hDrop, 0xFFFFFFFF, NULL, 0 );

	ppDroppedItems = ( char** )malloc( sizeof(char*)*( nDroppedItemsCount + 1 ) );
	
	if ( ppDroppedItems == NULL )
	{
		return( FALSE );
	}

	ppDroppedItems[ nDroppedItemsCount ] = NULL;
	
	for( iItem = 0; iItem < nDroppedItemsCount; ++iItem ) 
	{
		DragQueryFileA( hDrop, iItem, szFilename, sizeof( szFilename ) );
		ppDroppedItems[ iItem ] = _strdup( szFilename );
	}
	
	nFilesCount = CountDroppedFiles( ppDroppedItems, nDroppedItemsCount );
	
	ppFiles = ( char** )malloc( sizeof( char *)* ( nFilesCount+1 ) );
	
	if ( ppFiles == NULL )
	{
		return( FALSE );
	}
	
	ppFiles[ nFilesCount] = NULL;

	ProcessDroppedItems( ppDroppedItems, nDroppedItemsCount, ppFiles );

	bSuccess = (BOOL)CallService( MS_CLIST_CONTACTFILESDROPPED, (WPARAM)pThumb->hContact, (LPARAM)ppFiles ); 

	// Cleanup
	for( iItem = 0; ppDroppedItems[ iItem ]; ++iItem ) 
	{
		free( ppDroppedItems[ iItem ] );
	}

	free( ppDroppedItems );

	for( iItem = 0; iItem < nFilesCount ; ++iItem ) 
	{
		free( ppFiles[ iItem ] );
	}

	free( ppFiles );

	return( bSuccess );
}


static int CountFiles( char *szItem )
{
	int nCount = 0;
	WIN32_FIND_DATAA	fd;
	
	HANDLE hFind = FindFirstFileA( szItem, &fd );

	if ( hFind != INVALID_HANDLE_VALUE )
	{
		do
		{
			if ( fd.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY )
			{
				// Skip parent directories
				if ( ( 0 != strcmp( fd.cFileName, "." ) ) &&
					 ( 0 != strcmp( fd.cFileName, ".." ) ) )
				{
					char szDirName[ MAX_PATH ];
					strncpy( szDirName, szItem, MAX_PATH - 1 );

					if ( NULL != strstr( szItem, "*.*" ) )
					{
						sprintf( szDirName + strlen( szDirName ) - 3, "%s\0", fd.cFileName );
					}
					
					++nCount;
					strcat( szDirName, "\\*.*" );
					nCount += CountFiles( szDirName );
				}
			}
			else
			{
				++nCount;
			}
		}
		while( FALSE != FindNextFileA( hFind, &fd ) );
	}

	return( nCount );
}



static void SaveFiles( char *szItem, char **ppFiles, int *pnCount )
{
	
	WIN32_FIND_DATAA	fd;

	HANDLE hFind = FindFirstFileA( szItem, &fd );

	if ( hFind != INVALID_HANDLE_VALUE )
	{
		do
		{
			if ( fd.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY )
			{
				// Skip parent directories
				if ( ( 0 != strcmp( fd.cFileName, "." ) ) &&
					 ( 0 != strcmp( fd.cFileName, ".." ) ) )
				{
					char szDirName[ MAX_PATH ];
					strncpy( szDirName, szItem, MAX_PATH - 1 );

					if ( NULL != strstr( szItem, "*.*" ) )
					{
						sprintf( szDirName + strlen( szDirName ) - 3, "%s\0", fd.cFileName );
					}
					
					ppFiles[ *pnCount ] = _strdup( szDirName );
					++( *pnCount );

					strcat( szDirName, "\\*.*" );
					SaveFiles( szDirName, ppFiles, pnCount );
					
				}
			}
			else
			{
				int	 nSize = sizeof(char) * ( strlen( szItem ) + strlen( fd.cFileName ) + sizeof( char ) );
				char *szFile = (char*) malloc( nSize ) ;
				
				strncpy( szFile, szItem, nSize - 1 ); 
				
				if ( NULL != strstr( szFile, "*.*" ) )
				{
					szFile[ strlen( szFile ) - 3 ] = '\0';
					strncat( szFile, fd.cFileName, MAX_PATH - 1 );
				}
								
				ppFiles[ *pnCount ] = szFile;
				++( *pnCount );
			}
		}
		while( FALSE != FindNextFileA( hFind, &fd ) );
	}
}


static void ProcessDroppedItems( char **ppDroppedItems, int nCount, char **ppFiles  )
{
	int i;
	int fileCount = 0;

	for( i = 0; i < nCount; ++i )
	{
		SaveFiles( ppDroppedItems[ i ], ppFiles, &fileCount );
	}
}


static int CountDroppedFiles( char **ppDroppedItems, int nCount  )
{
	int fileCount = 0;
	int i;
	
	for( i = 0; i < nCount; ++i )
	{
		fileCount += CountFiles( ppDroppedItems[ i ] );
	}

	return( fileCount );
}


///////////////////////////////////////////////////////////////////////////////
// Init/destroy
void InitFileDropping()
{
	OleInitialize( NULL );
}

void FreeFileDropping(void)
{
	OleUninitialize();
}

void RegisterFileDropping( HWND hwnd, CDropTarget* pdropTarget )
{
	RegisterDragDrop( hwnd, (IDropTarget*)pdropTarget );
}

void UnregisterFileDropping( HWND hwnd )
{
	RevokeDragDrop( hwnd );
}
