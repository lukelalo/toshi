typedef int (*_asFunc)(_asNode *, _asNode *, int, void *);

int CAStar::udFunc(_asFunc func, _asNode *param1, _asNode *param2, int data, void *cb)
{
	if (func) return func(param1, param2, data, cb);

	return 1;
}

/*int Funcion_Coste( _asNode *param1, _asNode *param2, int data, void *cb);
int Funcion_Validez( _asNode *param1, _asNode *param2, int data, void *cb);*/