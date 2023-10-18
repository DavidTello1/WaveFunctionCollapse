class Event 
{
protected:
    virtual ~Event() {};
};

//------------------------
struct EventWindowResize : public Event
{
    EventWindowResize(int w, int h) : width(w), height(h) {};
    int width;
    int height;
};

struct EventCameraZoom : public Event
{
    EventCameraZoom(int z) : zoom(z) {};
    int zoom;
};

struct EventPanelToggled : public Event
{
    EventPanelToggled(bool isOpen) : isOpen(isOpen) {};
    bool isOpen;
};