/*
 * PS3 VKCube Port - main.c
 * 
 * This file implements a spinning 3D cube on the PlayStation 3 PPE (PowerPC Processor Element)
 * using the Tiny3D library from the PSL1GHT SDK. 
 *
 * Tiny3D provides a simple fixed-function style 3D pipeline on top of the PS3's RSX (Reality Synthesizer)
 * graphics processor.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <malloc.h>
#include <math.h>
#include <io/pad.h>
#include <sysutil/sysutil.h>
#include <tiny3d.h>
#include <matrix.h>

/* Global loop control variable */
static int running = 1;

/**
 * print_matrix
 *
 * Helper function to output the 4x4 matrix components to the standard output
 * for diagnosing mathematical transformations, scale degradation, or floating point errors.
 */
static void print_matrix(const char *name, MATRIX m) {
    printf("DEBUG MATRIX %s:\n", name);
    for (int i = 0; i < 4; i++) {
        printf("  [%f, %f, %f, %f]\n", m.data[i][0], m.data[i][1], m.data[i][2], m.data[i][3]);
    }
}

/**
 * sysutil_callback
 *
 * System event callback for PlayStation 3. This handles events triggered by the OS, 
 * such as when the user requests to exit the game back to the XMB (XrossMediaBar).
 *
 * @param status Event status code.
 * @param param Event parameter.
 * @param usrdata User data pointer.
 */
static void sysutil_callback(u64 status, u64 param, void *usrdata) {
    switch (status) {
        case SYSUTIL_EXIT_GAME:
            /* Log the exit request event dispatched by the PS3 OS to determine if RPCS3 is requesting immediate shutdown */
            printf("DEBUG: SYSUTIL_EXIT_GAME received from the operating system callback!\n");
            /* Flag the main loop to terminate gracefully */
            running = 0;
            break;
        default:
            break;
    }
}

static void draw_cube(MATRIX modelview, MATRIX rot) {
    /* Define vertex positions for the 6 faces of the cube, 4 vertices per face */
    static const float vVertices[] = {
        // front
        -1.0f, -1.0f, +1.0f,
        +1.0f, -1.0f, +1.0f,
        +1.0f, +1.0f, +1.0f,
        -1.0f, +1.0f, +1.0f,
        // back
        -1.0f, -1.0f, -1.0f,
        -1.0f, +1.0f, -1.0f,
        +1.0f, +1.0f, -1.0f,
        +1.0f, -1.0f, -1.0f,
        // top
        -1.0f, +1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        // bottom
        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
        // left
        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,
        // right
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    /* Define surface normals for each face (replicated per vertex in the original) */
    static const float vNormals[] = {
        // front
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        // back
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f,
        // top
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        // bottom
        0.0f, -1.0f, 0.0f,
        0.0f, -1.0f, 0.0f,
        0.0f, -1.0f, 0.0f,
        0.0f, -1.0f, 0.0f,
        // left
        -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,
        // right
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f
    };

    /* Define vertex colors for the 6 faces of the cube, 4 vertices per face to create a vibrant gradient.
       Each face has individual color gradients to mimic the colorful, multi-color look of the original VKCube. */
    static const float vColors[] = {
        // front (Yellow, Red, Purple, Cyan)
        1.0f, 1.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        0.5f, 0.0f, 1.0f,
        0.0f, 1.0f, 1.0f,
        // back (Green, Blue, Magenta, White)
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        // top (Red, Blue, Cyan, Green)
        1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 1.0f,
        0.0f, 1.0f, 0.0f,
        // bottom (Yellow, Orange, Pink, Purple)
        1.0f, 1.0f, 0.0f,
        1.0f, 0.5f, 0.0f,
        1.0f, 0.0f, 0.5f,
        0.5f, 0.0f, 0.5f,
        // left (Cyan, White, Magenta, Blue)
        0.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        // right (Green, Yellow, Orange, Red)
        0.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        1.0f, 0.5f, 0.0f,
        1.0f, 0.0f, 0.0f
    };

    tiny3d_SetPolygon(TINY3D_QUADS);

    /* Loop through the 6 faces of the cube to calculate Gouraud shading per vertex */
    for (int f = 0; f < 6; f++) {
        for (int v = 0; v < 4; v++) {
            int idx = f * 4 + v;
            VECTOR vertex_pos = { vVertices[idx*3], vVertices[idx*3+1], vVertices[idx*3+2] };
            VECTOR normal = { vNormals[idx*3], vNormals[idx*3+1], vNormals[idx*3+2] };

            /* Transform normal to eye space using the rotation matrix (transpose of inverse for orthogonal) */
            VECTOR Ne = MatrixVectorMultiply3x3(rot, normal);
            float n_len = sqrtf(Ne.x*Ne.x + Ne.y*Ne.y + Ne.z*Ne.z);
            if (n_len > 0.0f) {
                Ne.x /= n_len; Ne.y /= n_len; Ne.z /= n_len;
            }

            /* Transform the vertex position to eye space */
            VECTOR Pe = MatrixVectorMultiply(modelview, vertex_pos);

            /* Calculate light direction vector from eye-space position to light source at (2.0, 2.0, -20.0) */
            /* Z coordinate of light is negative since Tiny3D camera looks down the positive Z axis */
            float lx = 2.0f - Pe.x;
            float ly = 2.0f - Pe.y;
            float lz = -20.0f - Pe.z;
            float llen = sqrtf(lx*lx + ly*ly + lz*lz);
            if (llen > 0.0f) {
                lx /= llen; ly /= llen; lz /= llen;
            }

            /* Compute dot product for diffuse lighting intensity */
            float diff = Ne.x * lx + Ne.y * ly + Ne.z * lz;
            if (diff < 0.0f) diff = 0.0f;

            /* Set ambient baseline component to 0.40f and diffuse weight to 0.60f.
               This ensures the cube faces are brightly lit and visible against the background. */
            float intensity = 0.40f + 0.60f * diff;
            if (intensity > 1.0f) intensity = 1.0f;

            /* Set base vertex colors from the vibrant multi-color array to render a colorful cube.
               This configuration applies custom base colors for each vertex to form smooth color gradients. */
            float vr = vColors[idx*3];
            float vg = vColors[idx*3+1];
            float vb = vColors[idx*3+2];

            /* Apply Gouraud shading intensity to the multi-color vertex color values */
            float r = vr * intensity;
            float g = vg * intensity;
            float b = vb * intensity;

            /* Use tiny3d_VertexFcolor to pass float values (r, g, b, a), which is immune to the
               endianness and byte-order issues of the PS3 Big-Endian PowerPC architecture. */
            tiny3d_VertexFcolor(r, g, b, 1.0f);
            tiny3d_VertexPos(vertex_pos.x, vertex_pos.y, vertex_pos.z);
        }
    }

    tiny3d_End();
}

int main(int argc, char *argv[]) {
    /* Initialize gamepad controller system for up to 7 pads */
    ioPadInit(7);

    /* Register OS callback slot 0 to receive system power/quit signals */
    sysUtilRegisterCallback(SYSUTIL_EVENT_SLOT0, sysutil_callback, NULL);

    /* Initialize Tiny3D engine with 1MB of vertex buffer memory allocation */
    if (tiny3d_Init(1024 * 1024) != TINY3D_OK) {
        printf("Failed to initialize Tiny3D.\n");
        return -1;
    }
    /* Log successful initialization of the graphics engine to ensure the application reaches the render loop */
    printf("DEBUG: Tiny3D graphics engine initialized successfully. Entering the main render loop.\n");

    float angle = 0.0f;
    padInfo padinfo;
    padData paddata;

    /* Initialize controller structures to zero to prevent uninitialized stack garbage */
    memset(&padinfo, 0, sizeof(padInfo));
    memset(&paddata, 0, sizeof(padData));

    /* Main rendering and input loop */
    while (running) {
        /* Process system callbacks (e.g. exit signals) */
        sysUtilCheckCallback();

        /* Query controller status */
        ioPadGetInfo(&padinfo);
        for (int i = 0; i < MAX_PADS; i++) {
            if (padinfo.status[i]) {
                /* Clear struct memory before querying data */
                memset(&paddata, 0, sizeof(padData));
                
                /* Only process input if query succeeds and returned length is greater than 0 */
                if (ioPadGetData(i, &paddata) == 0 && paddata.len > 0) {
                    /* Quit application if Start, Cross, or Circle is pressed */
                    if (paddata.BTN_START || paddata.BTN_CROSS || paddata.BTN_CIRCLE) {
                        /* Log which specific button triggered the exit event to diagnose spontaneous inputs */
                        printf("DEBUG: Gamepad input triggered exit. Port: %d, START: %d, CROSS: %d, CIRCLE: %d\n",
                               i, paddata.BTN_START, paddata.BTN_CROSS, paddata.BTN_CIRCLE);
                        running = 0;
                    }
                }
            }
        }

        /* Clear frame buffers (Color + Depth/Z-Buffer) with dark gray background (ARGB format) */
        tiny3d_Clear(0xff323232, TINY3D_CLEAR_ALL);

        /* Set Tiny3D context mode to 3D rendering */
        tiny3d_Project3D();

        /* Set projection matrix aspect ratio to 1.0f. Tiny3D's viewport scaling handles the 16:9/4:3
           aspect ratio correction, so passing 1.0f prevents double-scaling and ensures a perfect cube. */
        float aspect = 1.0f;
        /* Log the detected display resolution and calculated aspect ratio to diagnose distortion */
        static int printed_res = 0;
        if (!printed_res) {
            printf("DEBUG: Display Resolution: %dx%d, Calculated Aspect (W/H): %f\n",
                   Video_Resolution.width, Video_Resolution.height, aspect);
            printed_res = 1;
        }

        /* Convert FOV to radians since Tiny3D's MatrixProjPerspective expects the angle in radians. */
        /* Passing 45.0f directly causes a massive zoom-out, rendering the cube extremely small. */
        float fov_rad = 45.0f * 3.14159265f / 180.0f;
        MATRIX projection = MatrixProjPerspective(fov_rad, aspect, 0.1f, 100.0f);
        tiny3d_SetProjectionMatrix(&projection);

        /* Construct a translation matrix translating the cube along positive Z (in front of camera) */
        /* because the Tiny3D projection matrix expects positive Z coordinates for visible geometry. */
        MATRIX trans = MatrixTranslation(0.0f, 0.0f, 5.0f);

        /* Compute rotations for single-axis rotation with no tilt to make the cube stand completely upright/straight */
        /* No tilt on X and Z axes, continuous rotation on Y axis */
        float degX = 0.0f;
        float degY = angle;
        float degZ = 0.0f;

        float radX = degX * 3.14159265f / 180.0f;
        float radY = degY * 3.14159265f / 180.0f;
        float radZ = degZ * 3.14159265f / 180.0f;

        MATRIX rotX = MatrixRotationX(radX);
        MATRIX rotY = MatrixRotationY(radY);
        MATRIX rotZ = MatrixRotationZ(radZ);

        /* Chain rotations to rotate around the local Y axis first, then apply global tilt */
        MATRIX tilt = MatrixMultiply(rotX, rotZ);
        MATRIX rot = MatrixMultiply(tilt, rotY);

        /* In row-vector matrix libraries (like Tiny3D), we must multiply rot by trans (rot * trans)
           to rotate the vertices first (spinning the cube in place) and then translate it to (0, 0, 5.0).
           Multiplying trans by rot (trans * rot) would translate first and then rotate, causing the cube to orbit out of the camera's view. */
        MATRIX modelview = MatrixMultiply(rot, trans);

        /* Print computed matrices periodically (every 30 frames) to trace mathematical decay or shear */
        static int frame_count = 0;
        if (frame_count % 30 == 0) {
            printf("DEBUG FRAME: %d, Angle: %f\n", frame_count, angle);
            print_matrix("rotX", rotX);
            print_matrix("rotY", rotY);
            print_matrix("rot", rot);
            print_matrix("modelview", modelview);
        }
        frame_count++;

        tiny3d_SetMatrixModelView(&modelview);

        /* Render the cube geometry with computed normal-based colors */
        draw_cube(modelview, rot);

        /* Swap frame buffers and wait for vertical blank synchronization (VBlank) */
        tiny3d_Flip();

        /* Increment rotation angle */
        angle += 1.0f;
        if (angle >= 36000.0f) {
            angle = 0.0f;
        }
    }

    /* Clean up systems and shut down Tiny3D graphics driver */
    tiny3d_Exit();
    ioPadEnd();
    sysUtilUnregisterCallback(SYSUTIL_EVENT_SLOT0);

    return 0;
}
