static void __exit hci_uart_exit(void)
{
	int err;

#ifdef CONFIG_BT_HCIUART_H4
	h4_deinit();
#endif
#ifdef CONFIG_BT_HCIUART_BCSP
	bcsp_deinit();
#endif
#ifdef CONFIG_BT_HCIUART_LL
	ll_deinit();
#endif
#ifdef CONFIG_BT_HCIUART_ATH3K
	ath_deinit();
#endif
#ifdef CONFIG_BT_HCIUART_3WIRE
	h5_deinit();
#endif
#ifdef CONFIG_BT_HCIUART_INTEL
	intel_deinit();
#endif
#ifdef CONFIG_BT_HCIUART_BCM
	bcm_deinit();
#endif
#ifdef CONFIG_BT_HCIUART_QCA
	qca_deinit();
#endif
#ifdef CONFIG_BT_HCIUART_AG6XX
	ag6xx_deinit();
#endif
#ifdef CONFIG_BT_HCIUART_MRVL
	mrvl_deinit();
#endif

	/* Release tty registration of line discipline */
	err = tty_unregister_ldisc(N_HCI);
	if (err)
		BT_ERR("Can't unregister HCI line discipline (%d)", err);
}
