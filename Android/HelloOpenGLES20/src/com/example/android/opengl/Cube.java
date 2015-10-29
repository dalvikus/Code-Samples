package com.example.android.opengl20;

class Cube
{
    private final Square mSquare1;
    private final Square mSquare2;
    private final Square mSquare3;
    private final Square mSquare4;
    private final Square mSquare5;
    private final Square mSquare6;
    Cube()
    {
        // front
        float[] squareCoords1 = {
            -0.5f,  0.5f, 0.5f,   // top left
            -0.5f, -0.5f, 0.5f,   // bottom left
             0.5f, -0.5f, 0.5f,   // bottom right
             0.5f,  0.5f, 0.5f    // top right
        };
        float[] color1 = {1.0f, 0.0f, 0.0f, 1.0f};
        mSquare1 = new Square(squareCoords1, color1);
        // back
        float[] squareCoords2 = {
             0.5f,  0.5f, -0.5f,   // top left
             0.5f, -0.5f, -0.5f,   // bottom left
            -0.5f, -0.5f, -0.5f,   // bottom right
            -0.5f,  0.5f, -0.5f    // top right
        };
        float[] color2 = {1.0f, 1.0f, 0.0f, 1.0f};
        mSquare2 = new Square(squareCoords2, color2);
        // left
        float[] squareCoords3 = {
            -0.5f,  0.5f, -0.5f,   // top left
            -0.5f, -0.5f, -0.5f,   // bottom left
            -0.5f, -0.5f,  0.5f,   // bottom right
            -0.5f,  0.5f,  0.5f    // top right
        };
        float[] color3 = {0.0f, 1.0f, 0.0f, 1.0f};
        mSquare3 = new Square(squareCoords3, color3);
        // right
        float[] squareCoords4 = {
             0.5f,  0.5f,  0.5f,   // top left
             0.5f, -0.5f,  0.5f,   // bottom left
             0.5f, -0.5f, -0.5f,   // bottom right
             0.5f,  0.5f, -0.5f    // top right
        };
        float[] color4 = {0.0f, 1.0f, 1.0f, 1.0f};
        mSquare4 = new Square(squareCoords4, color4);
        // top
        float[] squareCoords5 = {
            -0.5f,  0.5f, -0.5f,   // top left
            -0.5f,  0.5f,  0.5f,   // bottom left
             0.5f,  0.5f,  0.5f,   // bottom right
             0.5f,  0.5f, -0.5f    // top right
        };
        float[] color5 = {0.0f, 0.0f, 1.0f, 1.0f};
        mSquare5 = new Square(squareCoords5, color5);
        // bottom
        float[] squareCoords6 = {
            -0.5f, -0.5f,  0.5f,   // top left
            -0.5f, -0.5f, -0.5f,   // bottom left
             0.5f, -0.5f, -0.5f,   // bottom right
             0.5f, -0.5f,  0.5f    // top right
        };
        float[] color6 = {1.0f, 0.0f, 1.0f, 1.0f};
        mSquare6 = new Square(squareCoords6, color6);
    }
    public void draw(float[] mvpMatrix) {
        mSquare1.draw(mvpMatrix);
        mSquare2.draw(mvpMatrix);
        mSquare3.draw(mvpMatrix);
        mSquare4.draw(mvpMatrix);
        mSquare5.draw(mvpMatrix);
        mSquare6.draw(mvpMatrix);
    }
}
