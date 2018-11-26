// ELM_TC=1 ELM_TEST_FOCUS=5 ./test_focus
// ELM_TC=1 NO_MAINLOOP=1 pref stat ./test_focus
// gcc -o test_focus test_focus.c `pkg-config --cflags --libs elementary ecore-input-evas`

#ifdef HAVE_CONFIG_H
# include "elementary_config.h"
#endif

#include <Elementary.h>
#include <Ecore_Input_Evas.h>


#define PSIZE 318


typedef struct
{
   Evas_Object *scroller;
   Evas_Object *first_it;
   Ecore_Timer *timer;
   Ecore_Event_Key key;
   int frames;
   int focus_max;
   int state;
   int total_count;
   unsigned long long total, min, max, first;
} Focus;

Evas_Object *_tbtn1, *_tbtn2, *_tbtnscr;

static Eina_Bool
_focus_test_cb(void *data)
{
   Focus *focus = data;

   if (!ecore_evas_focus_get(ecore_evas_ecore_evas_get(evas_object_evas_get(focus->scroller))))
     return EINA_TRUE;

   if (focus->state == -1 || focus->state == 0)
     focus->state = 1;

   if (focus->state & 1)
     {
        focus->key.keyname = "Down";
        focus->key.key = "Down";
        focus->key.keycode = 116;
     }
   else
     {
        focus->key.keyname = "Up";
        focus->key.key = "Up";
        focus->key.keycode = 111;
     }
   struct timespec t0, t1;
   unsigned long long t1ll, t0ll, trll, ret;

   clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t0);
   ecore_event_evas_key_down(NULL, 0, &(focus->key));
   clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t1);

   t0ll = ((unsigned long long) t0.tv_sec * 1000000000ULL) + t0.tv_nsec;
   t1ll = ((unsigned long long) t1.tv_sec * 1000000000ULL) + t1.tv_nsec;
   trll = t1ll - t0ll;
   if (focus->min > trll) focus->min = trll;
   if (focus->max < trll) focus->max = trll;
   focus->total += trll;
   focus->total_count++;
   if (focus->total_count == 1)
     focus->first = trll;
   //printf("time : %llu\n", trll);

   ret = focus->total / focus->total_count;

   if (focus->state > focus->focus_max * 2)
     {
        printf(" first : %llu nsec, min : %llu nsec, max : %llu nsec\n", focus->first, focus->min, focus->max);
        printf(" average : %llu nsec (total : %llu nsec / count : %d)\n", ret, focus->total, focus->total_count);
        elm_exit();
     }
   return EINA_TRUE;
}

static void
_scroll2_del_cb(void *data, Evas *e EINA_UNUSED,
                Evas_Object *obj EINA_UNUSED, void *event_info EINA_UNUSED)
{
   Focus *focus = data;

   ecore_timer_del(focus->timer);
   free(focus);
}

static void
_focus_state_change_cb(void *data EINA_UNUSED, Evas_Object *obj, void *event_info EINA_UNUSED)
{
   Focus *focus = data;

   focus->state++;
}

static void
_test1_cb(void *data EINA_UNUSED, Evas_Object *obj, void *event_info EINA_UNUSED)
{
   elm_object_focus_set(_tbtnscr, EINA_TRUE);
   elm_object_focus_set(_tbtn2, EINA_TRUE);
   elm_object_focus_set(_tbtn1, EINA_TRUE);
}

void
focus_test1()
{
   Evas_Object *win, *bt, *bx, *bx2, *sc, *tb, *tb2, *rc;
   Focus *focus;
   int i, j;

   focus = calloc(1, sizeof(Focus));
   focus->max = 0;
   focus->min = ULONG_MAX;

   win = elm_win_util_standard_add("scroller2", "Scroller 2");
   elm_win_autodel_set(win, EINA_TRUE);

   bx = elm_box_add(win);
   evas_object_size_hint_weight_set(bx, EVAS_HINT_EXPAND, 0.0);
   evas_object_size_hint_align_set(bx, EVAS_HINT_FILL, 0.0);

   /* { */
   for (i = 0; i < 3; i++)
     {
        bt = elm_button_add(win);
        elm_object_text_set(bt, "Vertical");
        evas_object_size_hint_weight_set(bt, EVAS_HINT_EXPAND, 0.0);
        evas_object_size_hint_align_set(bt, EVAS_HINT_FILL, 0.5);
        elm_box_pack_end(bx, bt);
        evas_object_show(bt);

        if (i == 0)
          {
             evas_object_smart_callback_add(bt, "focused", _focus_state_change_cb, focus);
             focus->first_it = bt;
          }
     }
   /* } */

   /* { */
   sc = elm_scroller_add(win);
   evas_object_size_hint_weight_set(sc, EVAS_HINT_EXPAND, 0.0);
   evas_object_size_hint_align_set(sc, EVAS_HINT_FILL, 0.5);
   elm_scroller_bounce_set(sc, EINA_TRUE, EINA_FALSE);
   elm_scroller_content_min_limit(sc, 0, 1);
   elm_box_pack_end(bx, sc);
   evas_object_show(sc);

   bx2 = elm_box_add(win);
   elm_box_horizontal_set(bx2, EINA_TRUE);

   for (i = 0; i < 10; i++)
     {
        bt = elm_button_add(win);
        elm_object_text_set(bt, "... Horizontal scrolling ...");
        elm_box_pack_end(bx2, bt);
        evas_object_show(bt);
     }

   elm_object_content_set(sc, bx2);
   evas_object_show(bx2);
   /* } */

   /* { */
   for (i = 0; i < 3; i++)
     {
        bt = elm_button_add(win);
        elm_object_text_set(bt, "Vertical");
        evas_object_size_hint_weight_set(bt, EVAS_HINT_EXPAND, 0.0);
        evas_object_size_hint_align_set(bt, EVAS_HINT_FILL, 0.5);
        elm_box_pack_end(bx, bt);
        evas_object_show(bt);
     }
   /* } */

   bt = elm_button_add(win);
   elm_object_text_set(bt, "Test button1");
   evas_object_size_hint_weight_set(bt, EVAS_HINT_EXPAND, 0.0);
   evas_object_size_hint_align_set(bt, EVAS_HINT_FILL, 0.5);
   evas_object_smart_callback_add(bt, "clicked", _test1_cb, NULL);
   elm_box_pack_end(bx, bt);
   evas_object_show(bt);
   _tbtn1 = bt;

   /* { */
   tb = elm_table_add(win);
   evas_object_size_hint_weight_set(tb, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(tb, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_box_pack_end(bx, tb);
   evas_object_show(tb);

   rc = evas_object_rectangle_add(evas_object_evas_get(win));
   evas_object_size_hint_min_set(rc, 200, 120);
   elm_table_pack(tb, rc, 0, 0, 1, 1);

   sc = elm_scroller_add(win);
   evas_object_size_hint_weight_set(sc, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(sc, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_table_pack(tb, sc, 0, 0, 1, 1);
   evas_object_show(sc);

   tb2 = elm_table_add(win);

   for (j = 0; j < 20; j++)
     {
        for (i = 0; i < 20; i++)
          {
             bt = elm_button_add(win);
             elm_object_text_set(bt, "Both");
             elm_table_pack(tb2, bt, i, j, 1, 1);
             evas_object_show(bt);
             if (i == 10 && j == 10) _tbtnscr = bt;
          }
     }

   elm_object_content_set(sc, tb2);
   evas_object_show(tb2);
   /* } */

   bt = elm_button_add(win);
   elm_object_text_set(bt, "Test button2");
   evas_object_size_hint_weight_set(bt, EVAS_HINT_EXPAND, 0.0);
   evas_object_size_hint_align_set(bt, EVAS_HINT_FILL, 0.5);
   elm_box_pack_end(bx, bt);
   evas_object_show(bt);
   _tbtn2 = bt;

   for (i = 0; i < 24; i++)
     {
        bt = elm_button_add(win);
        elm_object_text_set(bt, "Vertical");
        evas_object_size_hint_weight_set(bt, EVAS_HINT_EXPAND, 0.0);
        evas_object_size_hint_align_set(bt, EVAS_HINT_FILL, 0.5);
        elm_box_pack_end(bx, bt);
        evas_object_show(bt);

        if (i == 23)
          evas_object_smart_callback_add(bt, "focused", _focus_state_change_cb, focus);
     }

   sc = elm_scroller_add(win);
   evas_object_size_hint_weight_set(sc, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_scroller_bounce_set(sc, EINA_FALSE, EINA_TRUE);
   elm_win_resize_object_add(win, sc);
   elm_object_content_set(sc, bx);
   evas_object_show(bx);
   evas_object_show(sc);

   focus->scroller = sc;

   evas_object_resize(win, 320, 480);
   evas_object_show(win);
   elm_object_focus_set(focus->first_it, EINA_TRUE);
   focus->state = -1;

   evas_object_event_callback_add(win, EVAS_CALLBACK_FREE, _scroll2_del_cb, focus);

   if (getenv("NO_MAINLOOP"))
     return;

   if (getenv("ELM_TEST_FOCUS"))
     {
        focus->key.window = elm_win_window_id_get(win);
        focus->key.event_window = elm_win_window_id_get(win);
        focus->focus_max = atoi(getenv("ELM_TEST_FOCUS"));
        focus->timer = ecore_timer_add(0.05, _focus_test_cb, focus);
     }
   elm_run();
}

void
focus_test2()
{
   Evas_Object *win, *bt, *bt1, *bx, *bx2, *sc, *tb, *tb2, *rc;
   Focus *focus;
   int i, j;

   focus = calloc(1, sizeof(Focus));
   focus->max = 0;
   focus->min = ULONG_MAX;

   win = elm_win_util_standard_add("scroller2", "Scroller 2");
   elm_win_autodel_set(win, EINA_TRUE);

   tb2 = elm_table_add(win);
#define MAXNUM 30
   for (j = 0; j < MAXNUM; j++)
     {
        for (i = 0; i < MAXNUM; i++)
          {
             bt = elm_button_add(win);
             elm_object_text_set(bt, "Both");
             elm_table_pack(tb2, bt, i, j, 1, 1);
             evas_object_show(bt);
             if (((j == 0) || (j == MAXNUM - 1)) && (i == 0))
               evas_object_smart_callback_add(bt, "focused", _focus_state_change_cb, focus);
             if (i == 0 && j == 0) bt1 = bt;
          }
     }
#undef MAXNUM
   elm_win_resize_object_add(win, tb2);
   evas_object_show(tb2);
   /* } */

   focus->scroller = win;
   evas_object_resize(win, 320, 480);
   evas_object_show(win);

   elm_object_focus_set(bt1, EINA_TRUE);
   evas_object_event_callback_add(win, EVAS_CALLBACK_FREE, _scroll2_del_cb, focus);
   focus->state = -1;

   if (getenv("NO_MAINLOOP"))
     return;

   if (getenv("ELM_TEST_FOCUS"))
     {
        focus->key.window = elm_win_window_id_get(win);
        focus->key.event_window = elm_win_window_id_get(win);
        focus->focus_max = atoi(getenv("ELM_TEST_FOCUS"));
        focus->timer = ecore_timer_add(0.05, _focus_test_cb, focus);
     }
   elm_run();
}

EAPI_MAIN int
elm_main(int argc EINA_UNUSED, char **argv EINA_UNUSED)
{
   const char *tc = getenv("ELM_TC");

   elm_config_focus_autoscroll_mode_set(ELM_FOCUS_AUTOSCROLL_MODE_BRING_IN);
   elm_config_window_auto_focus_animate_set(EINA_FALSE);
   elm_config_focus_highlight_animate_set(EINA_TRUE);

   if (*tc == '1')
     focus_test1();
   else if (*tc == '2')
     focus_test2();
}
ELM_MAIN()
