  static void  Ins_SZP2( INS_ARG )
  {
    switch ( args[0] )
    {
    case 0:
      CUR.zp2 = CUR.twilight;
      break;

    case 1:
      CUR.zp2 = CUR.pts;
      break;

    default:
      CUR.error = TT_Err_Invalid_Reference;
      return;
    }

    CUR.GS.gep2 = (Int)(args[0]);
  }
