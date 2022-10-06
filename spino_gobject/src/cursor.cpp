#include "cursor_private.h"

G_BEGIN_DECLS


G_DEFINE_TYPE(SpinoCursor, spino_cursor, G_TYPE_OBJECT)


static void spino_cursor_class_init(SpinoCursorClass* klass) 
{

}

static void spino_cursor_init(SpinoCursor* self) 
{

}

SpinoCursor* spino_cursor_new(std::unique_ptr<Spino::Cursor>& cursor)
{
    SpinoCursor* cur = (SpinoCursor*)g_object_new(SPINO_TYPE_CURSOR, NULL);
    cur->priv = std::move(cursor);
    return cur;
}

gchar* spino_cursor_next(SpinoCursor* self)
{
    return g_strdup(self->priv->next().c_str());
}

gboolean spino_cursor_has_next(SpinoCursor* self) 
{
    return self->priv->has_next();
}

SpinoCursor* spino_cursor_set_limit(SpinoCursor* self, guint limit)
{
    self->priv->set_limit(limit);
    return self;
}

G_END_DECLS
