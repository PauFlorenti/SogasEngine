#pragma once

namespace pinut::resources
{
class CommandBuffer;
}
namespace sogas
{
namespace modules
{
class IModule
{
  public:
    IModule(const std::string& name) : name(name){};

    const std::string& get_name() const
    {
        return name;
    }
    bool get_is_active() const
    {
        return is_active;
    }

  protected:
    virtual bool start()                                            = 0;
    virtual void stop()                                             = 0;
    virtual void update(f32 delta_time)                             = 0;
    virtual void render()                                           = 0;
    virtual void render_ui()                                        = 0;
    virtual void render_debug(pinut::resources::CommandBuffer* cmd) = 0;
    virtual void resize_window(u32 width, u32 height)               = 0;

  private:
    bool do_start()
    {
        ASSERT(is_active == false);
        if (is_active)
            return false;

        const bool ok = start();
        if (ok)
        {
            is_active = true;
        }
        return ok;
    }

    void do_stop()
    {
        ASSERT(is_active);
        if (!is_active)
            return;

        stop();
        is_active = false;
    }

    friend class ModuleManager;

    std::string name;
    bool        is_active{false};
};

using VectorModules = std::vector<std::shared_ptr<IModule>>;
} // namespace modules
} // namespace sogas
