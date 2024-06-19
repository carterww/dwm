/* See LICENSE file for copyright and license details. */

#ifdef LAPTOP_BUILD
#include <X11/XF86keysym.h>
#endif

#define MAINFONT "RobotoMono Nerd Font"
#define ICONFONT "Font Awesome 6 Free"

#define GAP 8
#define TERM "urxvt"
#define BROWSER "firefox"
#define BG_IMG_PATH "/usr/share/pixmaps/default-background.jpg"

/* appearance */
static const unsigned int borderpx  = 1;        /* border pixel of windows */
static const unsigned int gappx     = GAP;      /* gap pixel between windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const int swallowfloating    = 0;        /* 1 means swallow floating windows by default */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
// Outside padding (barpading patch)
static const int vertpad            = GAP;      /* vertical padding of bar */
static const int sidepad            = GAP;      /* horizontal padding of bar */
// Inside padding (statuspadding patch)
static const int horizpadbar        = GAP * 1.5;/* horizontal padding for statusbar */
static const int vertpadbar         = GAP * 1.5;/* vertical padding for statusbar */
static const char *fonts[]          = { MAINFONT":size=12:style=Regular", ICONFONT":size=12" };
static const char dmenufont[]       =  MAINFONT"size=12:style=Regular";
static const char col_black[]       = "#000000";
static const char col_white[]       = "#ffffff";
static const char col_gray1[]       = "#222222";
static const char col_gray2[]       = "#444444";
static const char col_gray3[]       = "#bbbbbb";
static const char col_gray4[]       = "#eeeeee";
static const char col_cyan[]        = "#24d8d2";
static const unsigned int baralpha = 0x80;
static const unsigned int borderalpha = OPAQUE;

static const char *colors[][3]      = {
	/*               fg         bg         border   */
	[SchemeNorm] = { col_gray3, col_gray1, col_gray2 }, // For inactive windows
	[SchemeSel]  = { col_gray4, col_gray2, col_cyan  }, // For active windows
};

static const unsigned int alphas[][3]      = {
    /*               fg      bg        border*/
    [SchemeNorm] = { OPAQUE, baralpha, borderalpha },
    [SchemeSel]  = { OPAQUE, baralpha, borderalpha },
};

/* tagging */
static const char *tags[] = { "󰈹", "󰙯", "", "?", "", "" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class     instance  title           tags mask  isfloating  isterminal  noswallow  monitor */
	{ "Gimp",    NULL,     NULL,           0,         0,          0,           0,        -1 },
	{ "Firefox", NULL,     NULL,           0,         0,          0,           1,        -1 },
	{ "URxvt",   NULL,     NULL,           1 << 2,    0,          1,           0,        -1 },
	{ NULL,      NULL,     "Event Tester", 0,         0,          0,           1,        -1 }, /* xev */
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */

// First layout is the default layout
static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "󱑝",      tile },
	{ "",      NULL },
	{ "",      monocle },
};

/* key definitions */
#define MODKEY Mod4Mask  // Windows key
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", col_gray1, "-nf", col_gray3, "-sb", col_gray2, "-sf", col_gray4, NULL };
static const char *termcmd[]  = { TERM, NULL };

/* Custom commands */
static const char *browsercmd[] = { BROWSER, NULL };
static const char *lockcmd[] = { "i3lock", "-t", "-i", BG_IMG_PATH, NULL };
static const char *pausecmd[] = { "playerctl", "play-pause", NULL };

static const char *volupcmd[] = { "pactl", "set-sink-volume", "@DEFAULT_SINK@", "+5%", NULL };
static const char *voldowncmd[] = { "pactl", "set-sink-volume", "@DEFAULT_SINK@", "-5%", NULL };
static const char *volmutecmd[] = { "pactl", "set-sink-mute", "@DEFAULT_SINK@", "toggle", NULL };

/* Custom commands to run on startup */
static const char *xsslockcmd[] = { "xss-lock", "--transfer-sleep-lock", "--", "i3lock", "--nofork", NULL };
static const char *setbgcmd[] = { "feh", "--bg-max", BG_IMG_PATH, NULL };
static const char *picomcmd[] = { "picom", NULL };
static const char **startupcmds[] = {
      xsslockcmd,
      setbgcmd,
      picomcmd,

      // Start terminal and browser on startup
      termcmd,
      browsercmd,
};

/* Brightness keys and commands */
#ifdef LAPTOP_BUILD
static cosnt KeySym briUp = XF86XK_MonBrightnessUp;
static const KeySym briDown = XF86XK_MonBrightnessDown;

// TODO: Check what I cmd i use on my laptop
static const char *briUpCmd[] = { "xbacklight", "-inc", "5", NULL };
static const char *briDownCmd[] = { "xbacklight", "-dec", "5", NULL };
#endif

/* Volume keys */
#ifdef LAPTOP_BUILD
static cosnt unsigned int volMod = 0; // No modifier
static const KeySym volUpKey = XF86XK_AudioRaiseVolume;
static const KeySym volDownKey = XF86XK_AudioLowerVolume;
static const KeySym volMuteKey = XF86XK_AudioMute;
#endif
#ifndef LAPTOP_BUILD
static const unsigned int volMod = MODKEY|Mod1Mask; // Windows key + Alt
static const KeySym volUpKey = XK_Right;
static const KeySym volDownKey = XK_Left;
static const KeySym volMuteKey = XK_m;
#endif

static const Key keys[] = {
	/* modifier                     key        function        argument */
	{ MODKEY,                       XK_d,      spawn,          {.v = dmenucmd } },      // Lauch dmenu with MODKEY + d
	{ MODKEY,                       XK_t,      spawn,          {.v = termcmd } },       // Lauch terminal with MODKEY + t
	{ MODKEY|ShiftMask,             XK_b,      togglebar,      {0} },                   // Toggle the to bar with MODKEY + shift + b
	{ MODKEY,                       XK_j,      focusstack,     {.i = +1 } },            // Move focus to the next window with MODKEY + j
	{ MODKEY,                       XK_k,      focusstack,     {.i = -1 } },            // Move focus to the previous window with MODKEY + k
	{ MODKEY,                       XK_n,      incnmaster,     {.i = +1 } },            // Increase the number of windows in the master area with MODKEY + n
	{ MODKEY|ShiftMask,             XK_n,      incnmaster,     {.i = -1 } },            // Decrease the number of windows in the master area with MODKEY + shift + n
	{ MODKEY,                       XK_Down,   setmfact,       {.f = -0.05} },          // Decrease the size of the master area with MODKEY + Down
	{ MODKEY,                       XK_Up,     setmfact,       {.f = +0.05} },          // Increase the size of the master area with MODKEY + Up
	{ MODKEY,                       XK_w,      zoom,           {0} },                   // Toggle window between master and stack with MODKEY + w
	{ MODKEY,                       XK_Tab,    view,           {0} },                   // Switch to previous tag with MODKEY + Tab
	{ MODKEY,                       XK_q,      killclient,     {0} },                   // Kill the active window MODKEY + q
	{ MODKEY|ShiftMask,             XK_t,      setlayout,      {.v = &layouts[0]} },    // Set the layout to tile with MODKEY + shift + t
        { MODKEY,                       XK_f,      togglefullscr,  {0} },                   // Toggle fullscreen with MODKEY + f
	{ MODKEY|ShiftMask,             XK_f,      setlayout,      {.v = &layouts[1]} },    // Set the layout to floating with MODKEY + shift + f
	{ MODKEY|ShiftMask,             XK_m,      setlayout,      {.v = &layouts[2]} },    // Set the layout to monocle with MODKEY + m
	{ MODKEY,                       XK_space,  setlayout,      {0} },                   // Toggle previous layout with MODKEY + space
	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },                   // Make active window float with MODKEY + Shift + space
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },           // View all windows with MODKEY + 0
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },           // Make focused window visible on all tags with MODKEY + Shift + 0
	{ MODKEY,                       XK_l,      focusmon,       {.i = -1 } },            // Move focus to previous monitor with MODKEY + l
	{ MODKEY,                       XK_h,      focusmon,       {.i = +1 } },            // Move focus to next monitor with MODKEY + h
	{ MODKEY|ShiftMask,             XK_l,      tagmon,         {.i = -1 } },            // Move window to previous monitor with MODKEY + Shift + l
	{ MODKEY|ShiftMask,             XK_h,      tagmon,         {.i = +1 } },            // Move window to next monitor with MODKEY + Shift + h
        { MODKEY,                       XK_minus,  setgaps,        {.i = -1 } },            // Decrease gaps with MODKEY + -
	{ MODKEY|ShiftMask,             XK_equal,  setgaps,        {.i = +1 } },            // Increase gaps with MODKEY + Shift + = (plus)
	{ MODKEY,                       XK_equal,  setgaps,        {.i = 0  } },            // Set gap to 0 with MODKEY + =
	TAGKEYS(                        XK_1,                      0)                       // Switch to tags with MODKEY + 1-9
	TAGKEYS(                        XK_2,                      1)                       // ...
	TAGKEYS(                        XK_3,                      2)                       // ...
	TAGKEYS(                        XK_4,                      3)                       // ...
	TAGKEYS(                        XK_5,                      4)                       // ...
	TAGKEYS(                        XK_6,                      5)                       // ...
        /*
	TAGKEYS(                        XK_7,                      6)                       // ...
	TAGKEYS(                        XK_8,                      7)                       // ...
	TAGKEYS(                        XK_9,                      8)                       // ...
        */

        // Custom commands
        { MODKEY,                       XK_b,      spawn,          {.v = browsercmd } },    // Lauch browser with MODKEY + b
        { MODKEY|ShiftMask,             XK_z,      spawn,          {.v = lockcmd } },       // Lock screen with MODKEY + Shift + z
        { MODKEY,                       XK_p,      spawn,          {.v = pausecmd } },      // Pause/play media with MODKEY + p

        // Volume keys
        { volMod,                       volUpKey,  spawn,          {.v = volupcmd } },      // Increase volume with volMod + volUpKey
        { volMod,                       volDownKey,spawn,          {.v = voldowncmd } },    // Decrease volume with volMod + volDownKey
        { volMod,                       volMuteKey,spawn,          {.v = volmutecmd } },    // Mute volume with volMod + volMuteKey
        
        // Brightness keys (laptop only)
#ifdef LAPTOP_BUILD
        { 0,                            briUp,     spawn,          {.v = briUpCmd } },      // Increase brightness with briUp
        { 0,                            briDown,   spawn,          {.v = briDownCmd } },    // Decrease brightness with briDown
#endif

	{ MODKEY|ShiftMask,             XK_e,      quit,           {0} },                   // Quit dwm with MODKEY + Shift + e
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static const Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

