class SelectionRectangle : public ManualObject
{
    Ogre::ManualObject* manual;
public:
    SelectionRectangle(const String& name)
        : ManualObject(name)
    {
        setRenderQueueGroup(RENDER_QUEUE_OVERLAY);
        setUseIdentityProjection(true);
        setUseIdentityView(true);
        setQueryFlags(0);
    }

    /**
    * Sets the corners of the SelectionRectangle.  Every parameter should be in the
    * range [0, 1] representing a percentage of the screen the SelectionRectangle
    * should take up.
    */
    void setCorners(float left, float top, float right, float bottom)
    {
        left   = left * 2 - 1;
        right  = right * 2 - 1;
        top    = 1 - top * 2;
        bottom = 1 - bottom * 2;
        clear();
        begin("SelectRect",  Ogre::RenderOperation::OT_TRIANGLE_FAN);

        position(left, top, -1);
        textureCoord(0, 0);
        colour(1.0f, 1.0f, 0.0f, 0.5f);


        position(left, bottom, -1);
        textureCoord(0, 1);
        colour(0.0f, 1.0f, 0.0f, 1.0f);

        position(right, bottom, -1);
        textureCoord(1, 1);
        colour(0.0f, 1.0f, 1.0f, 0.5f);

        position(right, top, -1);
        textureCoord(1, 0);
        colour(1.0f, 0.0f, 1.0f, 0.5f);

        // position(left, top, -1);
        // textureCoord(0,1);
        // colour(1.0f,1.0f,1.0f,1.0f);
        /*
         index(0);
                         index(1);
                         index(2);

                         index(0);
                         index(2);
                        index(3); */


        end();

        AxisAlignedBox box;
        box.setInfinite();
        setBoundingBox(box);
    }

    void setCorners(const Vector2& topLeft, const Vector2& bottomRight)
    {
        setCorners(topLeft.x, topLeft.y, bottomRight.x, bottomRight.y);
    }
};
