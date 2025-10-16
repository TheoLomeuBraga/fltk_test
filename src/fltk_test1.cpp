#include <FL/Fl_Double_Window.H>
#include <FL/Fl.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/fl_ask.H>
#include <FL/filename.H>
#include <FL/fl_string_functions.h>
#include <FL/Fl_Text_Buffer.H>
#include <FL/Fl_Text_Editor.H>
#include <FL/Fl_Native_File_Chooser.H>
#include <FL/platform.H>
#include <errno.h>

Fl_Menu_Bar *app_menu_bar = NULL;
bool text_changed = false;
char app_filename[FL_PATH_MAX] = "";

Fl_Text_Editor *app_editor = NULL;
Fl_Text_Editor *app_split_editor = NULL; // for later
Fl_Text_Buffer *app_text_buffer = NULL;

Fl_Double_Window *app_window = NULL;

void update_title()
{
  const char *fname = NULL;
  if (app_filename[0])
    fname = fl_filename_name(app_filename);
  if (fname)
  {
    char buf[FL_PATH_MAX + 3];
    buf[FL_PATH_MAX + 2] = '\0'; // ensure that the buffer is always terminated
    if (text_changed)
    {
      snprintf(buf, FL_PATH_MAX + 2, "%s *", fname);
    }
    else
    {
      snprintf(buf, FL_PATH_MAX + 2, "%s", fname);
    }
    app_window->copy_label(buf);
  }
  else
  {
    app_window->label("FLTK Editor");
  }
}

void set_changed(bool v)
{
  if (v != text_changed)
  {
    text_changed = v;
    update_title();
  }
}

void set_filename(const char *new_filename)
{
  if (new_filename)
  {
    fl_strlcpy(app_filename, new_filename, FL_PATH_MAX);
  }
  else
  {
    app_filename[0] = 0;
  }
  update_title();
}

void menu_quit_callback(Fl_Widget *, void *)
{
  Fl::hide_all_windows();
}

void tut1_build_app_window()
{
  app_window = new Fl_Double_Window(640, 480, "FLTK Editor");
}

void tut2_build_app_menu_bar()
{
  app_window->begin();
  app_menu_bar = new Fl_Menu_Bar(0, 0, app_window->w(), 25);
  app_menu_bar->add("File/Quit Editor", FL_COMMAND + 'q', menu_quit_callback);
  app_menu_bar->add("File/Do thing", FL_COMMAND + 'q', menu_quit_callback);

  app_window->callback(menu_quit_callback);
  app_window->end();
}

void text_changed_callback(int, int n_inserted, int n_deleted, int, const char *, void *)
{
  if (n_inserted || n_deleted)
    set_changed(true);
}

void menu_new_callback(Fl_Widget *, void *)
{
  app_text_buffer->text("");
  set_changed(false);
}

void tut3_build_main_editor()
{
  app_window->begin();
  app_text_buffer = new Fl_Text_Buffer();
  app_text_buffer->add_modify_callback(text_changed_callback, NULL);
  app_editor = new Fl_Text_Editor(0, app_menu_bar->h(),
                                  app_window->w(), app_window->h() - app_menu_bar->h());
  app_editor->buffer(app_text_buffer);
  app_editor->textfont(FL_COURIER);
  app_window->resizable(app_editor);
  app_window->end();
  int ix = app_menu_bar->find_index(menu_quit_callback);
  app_menu_bar->insert(ix, "New", FL_COMMAND + 'n', menu_new_callback);
}

void menu_save_as_callback(Fl_Widget *, void *)
{
  Fl_Native_File_Chooser file_chooser;
  file_chooser.title("Save File As...");
  file_chooser.type(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
  if (app_filename[0])
  {
    char temp_filename[FL_PATH_MAX];
    fl_strlcpy(temp_filename, app_filename, FL_PATH_MAX);
    const char *name = fl_filename_name(temp_filename);
    if (name)
    {
      file_chooser.preset_file(name);
      temp_filename[name - temp_filename] = 0;
      file_chooser.directory(temp_filename);
    }
  }
  if (file_chooser.show() == 0)
  {
    app_text_buffer->savefile(file_chooser.filename());
    set_filename(file_chooser.filename());
    set_changed(false);
  }
}

void menu_save_callback(Fl_Widget *, void *)
{
  Fl_Native_File_Chooser file_chooser;
  if (!app_filename[0])
  {
    menu_save_as_callback(NULL, NULL);
  }
  else
  {
    app_text_buffer->savefile(file_chooser.filename());
    set_changed(false);
  }
}

void load(const char *filename)
{
  if (app_text_buffer->loadfile(filename) == 0)
  {
    set_filename(filename);
    set_changed(false);
  }
  else
  {
    fl_alert("Failed to load file\n%s\n%s",
             filename,
             strerror(errno));
  }
}

void menu_open_callback(Fl_Widget *, void *)
{
  if (text_changed)
  {
    int r = fl_choice("The current file has not been saved.\n"
                      "Would you like to save it now?",
                      "Cancel", "Save", "Don't Save");
    if (r == 2)
      return;
    if (r == 1)
      menu_save_callback(NULL,NULL);
  }
  Fl_Native_File_Chooser file_chooser;
  file_chooser.title("Open File...");
  file_chooser.type(Fl_Native_File_Chooser::BROWSE_FILE);
  if (app_filename[0])
  {
    char temp_filename[FL_PATH_MAX];
    fl_strlcpy(temp_filename, app_filename, FL_PATH_MAX);
    const char *name = fl_filename_name(temp_filename);
    if (name)
    {
      file_chooser.preset_file(name);
      temp_filename[name - temp_filename] = 0;
      file_chooser.directory(temp_filename);
    }
  }
  if (file_chooser.show() == 0)
    load(file_chooser.filename());
}

void tut4_add_file_support()
{
  int ix = app_menu_bar->find_index(menu_quit_callback);
  app_menu_bar->insert(ix, "Open", FL_COMMAND + 'o', menu_open_callback, NULL, FL_MENU_DIVIDER);
  app_menu_bar->insert(ix + 1, "Save", FL_COMMAND + 's', menu_save_callback);
  app_menu_bar->insert(ix + 2, "Save as...", FL_COMMAND + 'S', menu_save_as_callback, NULL, FL_MENU_DIVIDER);
}

int args_handler(int argc, char **argv, int &i) {
  if (argv && argv[i] && argv[i][0]!='-') {
    load(argv[i]);
    i++;
    return 1;
  }
  return 0;
}

int tut4_handle_commandline_and_run(int &argc, char **argv) {
  int i = 0;
  Fl::args_to_utf8(argc, argv);
  Fl::args(argc, argv, i, args_handler);
  fl_open_callback(load);
  app_window->show(argc, argv);
  return Fl::run();
}

int main(int argc, char **argv)
{
  tut1_build_app_window();
  tut2_build_app_menu_bar();
  tut3_build_main_editor();
  tut4_add_file_support();
  app_window->show(argc, argv);
  return Fl::run();
  return tut4_handle_commandline_and_run(argc, argv);
}