
typedef struct _txhostinfo {
	void	*pHostData;
	BOOL	(*pAddPage)(void *pHostData, PROPSHEETPAGE *ppsp, void *pClientObject);
	void	(*pRemovePage)(void *pHostData, void *pClientObject);
	void	*(*pGetActivePage)(void *pHostData);
	LONG	(*pGetPageCount)(void *pHostData);
	BOOL	(*pSubclassColorButton)(void *pHostData, HWND hwndCtrl, COLORREF clrInitial);
	COLORREF	(*pSetButtonColor)(void *pHostData, HWND hwndCtrl, COLORREF clrNew);
	COLORREF	(*pGetButtonColor)(void *pHostData, HWND hwndCtrl);
	COLORREF	(*pChooseColor)(void *pHostData, COLORREF clrInitial);
}	TXHOSTINFO, *LPTXHOSTINFO;
