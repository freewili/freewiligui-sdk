#include <freewiligui/plugin.h>
#include <assert.h>
#include "freewiligui_browser.h"
int main(void) {
    FwGuiPluginBrowserInfo b = *FWGUI_BROWSER_INFO;
    FwGuiPluginDesc d = {0};
    d.struct_size = sizeof d; d.id="test.browser"; d.name="Browser";
    d.version="1.0"; d.author="FreeWili"; d.description="Browser metadata test"; d.browser=&b;
    assert(fwguiPluginDescError(&d)==NULL);
    d.description="  "; assert(fwguiPluginDescError(&d)!=NULL); d.description="Test";
    d.browser=NULL; assert(fwguiPluginDescError(&d)!=NULL); d.browser=&b;
    d.struct_size=offsetof(FwGuiPluginDesc,browser); assert(fwguiPluginDescError(&d)!=NULL);d.struct_size=sizeof d;
    b.kinds=0;assert(fwguiBrowserInfoError(&b)!=NULL);b.kinds=0x80;assert(fwguiBrowserInfoError(&b)!=NULL);b.kinds=FWGUI_PLUGIN_KIND_PANEL_CONTROL;
    b.help_markdown=" \n";assert(fwguiBrowserInfoError(&b)!=NULL);b=*FWGUI_BROWSER_INFO;
    b.thumbnail_png_size=0;assert(fwguiBrowserInfoError(&b)!=NULL);b=*FWGUI_BROWSER_INFO;
    b.thumbnail_png_size=FWGUI_BROWSER_MAX_PNG_BYTES+1;assert(fwguiBrowserInfoError(&b)!=NULL);b=*FWGUI_BROWSER_INFO;
    { unsigned char bad[33]={0};b.thumbnail_png=bad;b.thumbnail_png_size=33;assert(fwguiBrowserInfoError(&b)!=NULL); }
    b=*FWGUI_BROWSER_INFO;b.struct_size=0;assert(fwguiBrowserInfoError(&b)!=NULL);
    { unsigned char bad[33]; memcpy(bad, FWGUI_BROWSER_INFO->thumbnail_png, 33);
      b=*FWGUI_BROWSER_INFO;b.thumbnail_png=bad;b.thumbnail_png_size=33;
      bad[16]=bad[17]=bad[18]=bad[19]=0; assert(fwguiBrowserInfoError(&b)!=NULL);
      bad[18]=0x13;bad[19]=0x89;assert(fwguiBrowserInfoError(&b)!=NULL); }
    { static char huge[1048578];memset(huge,'x',sizeof huge-1);huge[sizeof huge-1]=0;
      b=*FWGUI_BROWSER_INFO;b.help_markdown=huge;assert(fwguiBrowserInfoError(&b)!=NULL); }
    return 0;
}
