xsltFreeExtDefList(xsltExtDefPtr extensiond)
{
    xsltExtDefPtr cur;

    while (extensiond != NULL) {
        cur = extensiond;
        extensiond = extensiond->next;
        xsltFreeExtDef(cur);
    }
}
