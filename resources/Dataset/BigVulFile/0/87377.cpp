Mat_VarReadData4(mat_t *mat,matvar_t *matvar,void *data,
      int *start,int *stride,int *edge)
{
    int err = 0;

    (void)fseek((FILE*)mat->fp,matvar->internal->datapos,SEEK_SET);

    switch( matvar->data_type ) {
        case MAT_T_DOUBLE:
        case MAT_T_SINGLE:
        case MAT_T_INT32:
        case MAT_T_INT16:
        case MAT_T_UINT16:
        case MAT_T_UINT8:
            break;
        default:
            return 1;
    }

    if ( matvar->rank == 2 ) {
        if ( (size_t)stride[0]*(edge[0]-1)+start[0]+1 > matvar->dims[0] )
            err = 1;
        else if ( (size_t)stride[1]*(edge[1]-1)+start[1]+1 > matvar->dims[1] )
            err = 1;
        if ( matvar->isComplex ) {
            mat_complex_split_t *cdata = (mat_complex_split_t*)data;
            size_t nbytes = Mat_SizeOf(matvar->data_type);
            err = SafeMulDims(matvar, &nbytes);
            if ( err ) {
                Mat_Critical("Integer multiplication overflow");
                return err;
            }

            ReadDataSlab2(mat,cdata->Re,matvar->class_type,matvar->data_type,
                matvar->dims,start,stride,edge);
            (void)fseek((FILE*)mat->fp,matvar->internal->datapos+nbytes,SEEK_SET);
            ReadDataSlab2(mat,cdata->Im,matvar->class_type,
                matvar->data_type,matvar->dims,start,stride,edge);
        } else {
            ReadDataSlab2(mat,data,matvar->class_type,matvar->data_type,
                matvar->dims,start,stride,edge);
        }
    } else if ( matvar->isComplex ) {
        mat_complex_split_t *cdata = (mat_complex_split_t*)data;
        size_t nbytes = Mat_SizeOf(matvar->data_type);
        err = SafeMulDims(matvar, &nbytes);
        if ( err ) {
            Mat_Critical("Integer multiplication overflow");
            return err;
        }

        ReadDataSlabN(mat,cdata->Re,matvar->class_type,matvar->data_type,
            matvar->rank,matvar->dims,start,stride,edge);
        (void)fseek((FILE*)mat->fp,matvar->internal->datapos+nbytes,SEEK_SET);
        ReadDataSlabN(mat,cdata->Im,matvar->class_type,matvar->data_type,
            matvar->rank,matvar->dims,start,stride,edge);
    } else {
        ReadDataSlabN(mat,data,matvar->class_type,matvar->data_type,
            matvar->rank,matvar->dims,start,stride,edge);
    }

    return err;
}
