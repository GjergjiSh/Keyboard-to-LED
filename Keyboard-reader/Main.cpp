#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/XInput.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

using namespace std;

int xi_opcode;

#define INVALID_EVENT_TYPE -1

static int key_press_type = INVALID_EVENT_TYPE;
static int key_release_type = INVALID_EVENT_TYPE;

static int register_events(Display *dpy,
                           XDeviceInfo *info,
                           char *dev_name)
{
  int number = 0; /* number of events registered */
  XEventClass event_list[7];
  int i;
  XDevice *device;
  Window root_win;
  unsigned long screen;
  XInputClassInfo *ip;

  screen = DefaultScreen(dpy);
  root_win = RootWindow(dpy, screen);

  device = XOpenDevice(dpy, info->id);

  if (!device)
  {
    printf("unable to open device '%s'\n", dev_name);
    return 0;
  }

  if (device->num_classes > 0)
  {
    for (ip = device->classes, i = 0; i < info->num_classes; ip++, i++)
    {
      switch (ip->input_class)
      {
      case KeyClass:
        DeviceKeyPress(device, key_press_type, event_list[number]);
        number++;
        DeviceKeyRelease(device, key_release_type, event_list[number]);
        number++;
        break;

      default:
        printf("unknown class\n");
        break;
      }
    }

    if (XSelectExtensionEvent(dpy, root_win, event_list, number))
    {
      printf("error selecting extended events\n");
      return 0;
    }
  }
  return number;
}

static void print_events(Display *dpy)
{
  XEvent Event;

  setvbuf(stdout, NULL, _IOLBF, 0);

  while (1)
  {
    XNextEvent(dpy, &Event);

    if ((Event.type == key_press_type) ||
        (Event.type == key_release_type))
    {
      int loop;
      XDeviceKeyEvent *key = (XDeviceKeyEvent *)&Event;

      printf("key %s %d ", (Event.type == key_release_type) ? "release" : "press  ",
             key->keycode);

      for (loop = 0; loop < key->axes_count; loop++)
      {
        printf("a[%d]=%d ", key->first_axis + loop, key->axis_data[loop]);
      }
      printf("\n");
    }
    else
    {
      printf("what's that %d\n", Event.type);
    }
  }
}

// Determining the version of the extension library installed for X11
int xinput_version(Display *display)
{
  XExtensionVersion *version;
  static int vers = -1;

  if (vers != -1)
    return vers;

  version = XGetExtensionVersion(display, INAME);

  if (version && (version != (XExtensionVersion *)NoSuchExtension))
  {
    vers = version->major_version;
    XFree(version);
  }

  return vers;
}

// Finding device information
XDeviceInfo *find_device_info(Display *display,
                              char *name)
{
  XDeviceInfo *devices;
  XDeviceInfo *found = NULL;
  int loop;
  int num_devices;
  int len = strlen(name);
  Bool is_id = True;
  XID id = (XID)-1;

  for (loop = 0; loop < len; loop++)
  {
    if (!isdigit(name[loop]))
    {
      is_id = False;
      break;
    }
  }

  if (is_id)
  {
    id = atoi(name);
  }

  devices = XListInputDevices(display, &num_devices);

  for (loop = 0; loop < num_devices; loop++)
  {
    if (((devices[loop].use >= IsXExtensionDevice)) &&
        ((!is_id && strcmp(devices[loop].name, name) == 0) ||
         (is_id && devices[loop].id == id)))
    {
      if (found)
      {
        fprintf(stderr,
                "Warning: There are multiple devices named '%s'.\n"
                "To ensure the correct one is selected, please use "
                "the device ID instead.\n\n",
                name);
        return NULL;
      }
      else
      {
        found = &devices[loop];
      }
    }
  }
  return found;
}

int test(Display *display, char *deviceId)
{
  XDeviceInfo *info;

  Bool handle_proximity = True;

  info = find_device_info(display, deviceId);

  if (!info)
  {
    printf("unable to find device '%s'\n", deviceId);
    exit(1);
  }
  else
  {
    if (register_events(display, info, deviceId))
      print_events(display);
    else
    {
      fprintf(stderr, "no event registered...\n");
      exit(1);
    }
  }

  return 0;
}

int main()
{
  Display *display;
  int event, error;

  // A pointer to the current display is initiated
  display = XOpenDisplay(NULL);
  if (display == NULL)
  {
    printf("Unable to connect to X server\n");
    exit(1);
  }

  // Checking for extensions
  if (!XQueryExtension(display, "XInputExtension", &xi_opcode, &event, &error))
  {
    printf("X Input extension not available.\n");
    exit(1);
  }

  // The version of the extension is checked, it must not be null
  if (!xinput_version(display))
  {
    printf("%s extension not available\n", INAME);
    exit(1);
  }

  char deviceId[10];
  sprintf(deviceId, "14");

  test(display, deviceId);

  XSync(display, False);
  XCloseDisplay(display);

  return 0;
}