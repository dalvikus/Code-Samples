/*
 * Copyright (C) 2011 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package com.example.android.opengl20;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;
import java.nio.IntBuffer;

import android.opengl.GLES20;

import android.content.Context;
import android.util.Log;
import java.util.Arrays;
import java.io.ObjectInputStream;
import java.io.BufferedInputStream;
import java.io.FileInputStream;
import java.io.InputStreamReader;
import java.util.zip.GZIPInputStream;
import java.io.IOException;
import java.io.InputStream;

/**
 * A two-dimensional square for use as a drawn object in OpenGL ES 2.0.
 */
public class Polygon {
    private static final String TAG = "Polygon";

    private static final String vertexShaderCode =
            // This matrix member variable provides a hook to manipulate
            // the coordinates of the objects that use this vertex shader
            "uniform mat4 uMVPMatrix;" +
            "attribute vec4 vPosition;" +
            "void main() {" +
            // The matrix must be included as a modifier of gl_Position.
            // Note that the uMVPMatrix factor *must be first* in order
            // for the matrix multiplication product to be correct.
            "  gl_Position = uMVPMatrix * vPosition;" +
            "}";
    private static final String fragmentShaderCode =
            "precision mediump float;" +
            "uniform vec4 vColor;" +
            "void main() {" +
            "  gl_FragColor = vColor;" +
            "}";

    private final FloatBuffer vertexBuffer; // tell if loadking is successful or not; see drawAll
    private final int mProgram;

    // number of coordinates per vertex in this array
    private static final int COORDS_PER_VERTEX = 3;
    private static final int vertexStride = COORDS_PER_VERTEX * 4; // 4 bytes per vertex

    private static float[] color = { 0.2f, 0.709803922f, 0.898039216f, 1.0f };

    class Draw
    {
        IntBuffer drawListBuffer;
        int[] drawOrder;
        float[] color;
    }

    private static final int I27 = 27;
    private static final float F3 = 1.0f / 2;
    private static final float ALPHA = 1.0f;
    private float[][] mColor_a = {
        {0 * F3, 0 * F3, 0 * F3, ALPHA},
        {0 * F3, 0 * F3, 1 * F3, ALPHA},
        {0 * F3, 0 * F3, 2 * F3, ALPHA},
        {0 * F3, 1 * F3, 0 * F3, ALPHA},
        {0 * F3, 1 * F3, 1 * F3, ALPHA},
        {0 * F3, 1 * F3, 2 * F3, ALPHA},
        {0 * F3, 2 * F3, 0 * F3, ALPHA},
        {0 * F3, 2 * F3, 1 * F3, ALPHA},
        {0 * F3, 2 * F3, 2 * F3, ALPHA},
        {1 * F3, 0 * F3, 0 * F3, ALPHA},
        {1 * F3, 0 * F3, 1 * F3, ALPHA},
        {1 * F3, 0 * F3, 2 * F3, ALPHA},
        {1 * F3, 1 * F3, 0 * F3, ALPHA},
        {1 * F3, 1 * F3, 1 * F3, ALPHA},
        {1 * F3, 1 * F3, 2 * F3, ALPHA},
        {1 * F3, 2 * F3, 0 * F3, ALPHA},
        {1 * F3, 2 * F3, 1 * F3, ALPHA},
        {1 * F3, 2 * F3, 2 * F3, ALPHA},
        {2 * F3, 0 * F3, 0 * F3, ALPHA},
        {2 * F3, 0 * F3, 1 * F3, ALPHA},
        {2 * F3, 0 * F3, 2 * F3, ALPHA},
        {2 * F3, 1 * F3, 0 * F3, ALPHA},
        {2 * F3, 1 * F3, 1 * F3, ALPHA},
        {2 * F3, 1 * F3, 2 * F3, ALPHA},
        {2 * F3, 2 * F3, 0 * F3, ALPHA},
        {2 * F3, 2 * F3, 1 * F3, ALPHA},
        {2 * F3, 2 * F3, 2 * F3, ALPHA},
    };
    private Draw[] mDraw_a;
    public Polygon(Context context, String objFile)
    {
        final String oviaFile = objFile + "_ovia.bin";  // change extension (.gz)
        // prepare shaders and OpenGL program
        int vertexShader = MyGLRenderer.loadShader(GLES20.GL_VERTEX_SHADER, vertexShaderCode);
        int fragmentShader = MyGLRenderer.loadShader(GLES20.GL_FRAGMENT_SHADER, fragmentShaderCode);

        mProgram = GLES20.glCreateProgram();             // create empty OpenGL Program
        GLES20.glAttachShader(mProgram, vertexShader);   // add the vertex shader to program
        GLES20.glAttachShader(mProgram, fragmentShader); // add the fragment shader to program
        GLES20.glLinkProgram(mProgram);                  // create OpenGL program executables

        float[] va3 = null;
        try {
            ObjectInputStream ois = new ObjectInputStream(new GZIPInputStream(new BufferedInputStream(context.getAssets().open(oviaFile))));
            va3 = (float[]) ois.readObject();
            Log.d(TAG, "IN: # of floats = " + va3.length);

            int k = ois.readInt();
            Log.d(TAG, "IN: # of groups = " + k);
            mDraw_a = new Draw[k];
            for (int i = 0; i < k; ++i) {
                String name = (String) ois.readObject();
                Log.d(TAG, String.format("IN: name: |%s|%n", name));
                mDraw_a[i] = new Draw();
                mDraw_a[i].drawOrder = (int[]) ois.readObject();
                Log.d(TAG, "IN: # of indides = " + mDraw_a[i].drawOrder.length);
                // (# of coordinate values * 2 bytes per int)
                ByteBuffer dlb = ByteBuffer.allocateDirect(mDraw_a[i].drawOrder.length * 4);
                dlb.order(ByteOrder.nativeOrder());
                mDraw_a[i].drawListBuffer = dlb.asIntBuffer();
                mDraw_a[i].drawListBuffer.put(mDraw_a[i].drawOrder);
                mDraw_a[i].drawListBuffer.position(0);

                mDraw_a[i].color = mColor_a[i % I27];
            }
            ois.close();
        } catch (IOException e) {     // new FileOutputStream(...)
            Log.e(TAG, "IOException: " + e.getMessage());
            va3 = null;
            mDraw_a = null;
        } catch (ClassNotFoundException e) {
            Log.e(TAG, "ClassNotFoundException: " + e.getMessage());
            va3 = null;
            mDraw_a = null;
        }
        if (va3 != null) {
            // initialize vertex byte buffer for shape coordinates
            // (# of coordinate values * 4 bytes per float)
            ByteBuffer bb = ByteBuffer.allocateDirect(va3.length * 4);
            bb.order(ByteOrder.nativeOrder());
            vertexBuffer = bb.asFloatBuffer();
            vertexBuffer.put(va3);
            vertexBuffer.position(0);
            va3 = null; // no more use
        } else {
            vertexBuffer = null;
        }
    }

    /**
     * Encapsulates the OpenGL ES instructions for drawing this shape.
     *
     * @param mvpMatrix - The Model View Project matrix in which to draw
     * this shape.
     */
    public void drawAll(float[] mvpMatrix) {
        if (vertexBuffer == null)
            return;
        // Add program to OpenGL environment
        GLES20.glUseProgram(mProgram);

        // get handle to vertex shader's vPosition member
        int mPositionHandle = GLES20.glGetAttribLocation(mProgram, "vPosition");

        // Enable a handle to the triangle vertices
        GLES20.glEnableVertexAttribArray(mPositionHandle);
        MyGLRenderer.checkGlError("glEnableVertexAttribArray");

        // Prepare the triangle coordinate data
////  public static void glVertexAttribPointer (int indx, int size, int type, boolean normalized, int stride, Buffer ptr)
        GLES20.glVertexAttribPointer(
                mPositionHandle, COORDS_PER_VERTEX,
                GLES20.GL_FLOAT, false,
                vertexStride, vertexBuffer);

        // get handle to shape's transformation matrix
        int mMVPMatrixHandle = GLES20.glGetUniformLocation(mProgram, "uMVPMatrix");
        MyGLRenderer.checkGlError("glGetUniformLocation");

        // Apply the projection and view transformation
////  public static void glUniformMatrix4fv (int location, int count, boolean transpose, float[] value, int offset)
        GLES20.glUniformMatrix4fv(mMVPMatrixHandle, 1, false, mvpMatrix, 0);
        MyGLRenderer.checkGlError("glUniformMatrix4fv");

        for (int i = 0; i < mDraw_a.length; ++i) {
                // get handle to fragment shader's vColor member
                int mColorHandle = GLES20.glGetUniformLocation(mProgram, "vColor");

                // Set color for drawing the triangle
        ////  public static void glUniform4iv (int location, int count, int[] v, int offset)
                GLES20.glUniform4fv(mColorHandle, 1, mColor_a[i], 0);
                // Draw the square
        ////  public static void glDrawElements (int mode, int count, int type, Buffer indices)
                GLES20.glDrawElements(
                        GLES20.GL_TRIANGLES, mDraw_a[i].drawOrder.length,
                        GLES20.GL_UNSIGNED_INT, mDraw_a[i].drawListBuffer);
        }

        // Disable vertex array
        GLES20.glDisableVertexAttribArray(mPositionHandle);
    }

    private IntBuffer drawListBuffer;
    private float[] mSquareCoords;
    private float[] mColor;
    private int[] mDrawOrder;
    public Polygon(float[] squareCoords, float[] color, int[] drawOrder)
    {
        mSquareCoords = Arrays.copyOf(squareCoords, squareCoords.length);
        mColor = Arrays.copyOf(color, color.length);
        mDrawOrder = Arrays.copyOf(drawOrder, drawOrder.length);

        // initialize vertex byte buffer for shape coordinates
        ByteBuffer bb = ByteBuffer.allocateDirect(
        // (# of coordinate values * 4 bytes per float)
                mSquareCoords.length * 4);
        bb.order(ByteOrder.nativeOrder());
        vertexBuffer = bb.asFloatBuffer();
        vertexBuffer.put(mSquareCoords);
        vertexBuffer.position(0);

        // initialize byte buffer for the draw list
        ByteBuffer dlb = ByteBuffer.allocateDirect(
                // (# of coordinate values * 2 bytes per short)
                mDrawOrder.length * 4);
        dlb.order(ByteOrder.nativeOrder());
        drawListBuffer = dlb.asIntBuffer();
        drawListBuffer.put(mDrawOrder);
        drawListBuffer.position(0);

        // prepare shaders and OpenGL program
        int vertexShader = MyGLRenderer.loadShader(
                GLES20.GL_VERTEX_SHADER,
                vertexShaderCode);
        int fragmentShader = MyGLRenderer.loadShader(
                GLES20.GL_FRAGMENT_SHADER,
                fragmentShaderCode);

        mProgram = GLES20.glCreateProgram();             // create empty OpenGL Program
        GLES20.glAttachShader(mProgram, vertexShader);   // add the vertex shader to program
        GLES20.glAttachShader(mProgram, fragmentShader); // add the fragment shader to program
        GLES20.glLinkProgram(mProgram);                  // create OpenGL program executables
    }
/*
    public void draw(float[] mvpMatrix)
    {
        // Add program to OpenGL environment
        GLES20.glUseProgram(mProgram);

        // get handle to vertex shader's vPosition member
        int mPositionHandle = GLES20.glGetAttribLocation(mProgram, "vPosition");

        // Enable a handle to the triangle vertices
        GLES20.glEnableVertexAttribArray(mPositionHandle);

        // Prepare the triangle coordinate data
////  public static void glVertexAttribPointer (int indx, int size, int type, boolean normalized, int stride, Buffer ptr)
        GLES20.glVertexAttribPointer(
                mPositionHandle, COORDS_PER_VERTEX,
                GLES20.GL_FLOAT, false,
                vertexStride, vertexBuffer);

        // get handle to fragment shader's vColor member
        int mColorHandle = GLES20.glGetUniformLocation(mProgram, "vColor");

        // Set color for drawing the triangle
////  public static void glUniform4iv (int location, int count, int[] v, int offset)
        GLES20.glUniform4fv(mColorHandle, 1, mColor, 0);

        // get handle to shape's transformation matrix
        int mMVPMatrixHandle = GLES20.glGetUniformLocation(mProgram, "uMVPMatrix");
        MyGLRenderer.checkGlError("glGetUniformLocation");

        // Apply the projection and view transformation
////  public static void glUniformMatrix4fv (int location, int count, boolean transpose, float[] value, int offset)
        GLES20.glUniformMatrix4fv(mMVPMatrixHandle, 1, false, mvpMatrix, 0);
        MyGLRenderer.checkGlError("glUniformMatrix4fv");

        // Draw the square
////  public static void glDrawElements (int mode, int count, int type, Buffer indices)
        GLES20.glDrawElements(
                GLES20.GL_TRIANGLES, mDrawOrder.length,
                GLES20.GL_UNSIGNED_INT, drawListBuffer);

        // Disable vertex array
        GLES20.glDisableVertexAttribArray(mPositionHandle);
    }
 */
}
