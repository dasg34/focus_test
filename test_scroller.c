// gcc -o test_scroller test_scroller.c `pkg-config --cflags --libs elementary`

#include <Elementary.h>

const char *str_tmp = "I am groot!! scroller focus scroller focus scroller " 
"focus scroller focus scroller focus scroller focus scroller focus scroller "
"focus scroller focus scroller focus scroller focus scroller focus scroller "
"focus scroller focus scroller focus scroller focus scroller focus scroller "
"focus scroller focus scroller focus scroller focus scroller focus scroller "
"focus scroller focus scroller focus scroller focus scroller focus scroller "
"focus scroller focus scroller focus scroller focus scroller focus scroller "
"focus scroller focus scroller focus scroller focus scroller focus scroller "
"focus scroller focus scroller focus scroller focus scroller focus scroller "
"focus scroller focus scroller focus scroller focus scroller focus scroller "
"focus scroller focus scroller focus scroller focus scroller focus scroller ";

EAPI_MAIN int
elm_main(int argc EINA_UNUSED, char **argv EINA_UNUSED)
{
   Evas_Object *win, *scr, *label, *box, *btn;

   elm_policy_set(ELM_POLICY_QUIT, ELM_POLICY_QUIT_LAST_WINDOW_CLOSED);

   win = elm_win_util_standard_add("scroller focus", "Scroller Focus Test");
   elm_win_autodel_set(win, EINA_TRUE);

   scr = elm_scroller_add(win);
   evas_object_size_hint_weight_set(scr, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_win_resize_object_add(win, scr);
   evas_object_show(scr);
   
   box = elm_box_add(scr);
   evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(box, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_show(box);
   elm_object_content_set(scr, box);

   label = elm_label_add(box);
   elm_object_text_set(label, str_tmp);
   elm_label_line_wrap_set(label, ELM_WRAP_MIXED);
   elm_label_wrap_width_set(label, 300);
   evas_object_size_hint_weight_set(label, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(label, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_show(label);
   elm_box_pack_end(box, label);

   btn = elm_button_add(box);
   evas_object_size_hint_weight_set(btn, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(btn, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_object_text_set(btn, "Goal is to read first word of label only using arrow key!");
   evas_object_show(btn);
   elm_box_pack_end(box, btn);

   evas_object_resize(win, 200, 200);
   evas_object_show(win);

   elm_run();

   return 0;
}
ELM_MAIN()
