//
//  gui.h
//  Special Relativity
//
//  Created by Antoni Wójcik on 11/01/2020.
//
//  Creates a basic GUI displaying info about current FOV of the camera, current time, current orientation of the camera, current position of the camera and FPS.
//

#ifndef gui_h
#define gui_h

#include <string>

// include FreeType libraries
#include <ft2build.h>
#include FT_FREETYPE_H

enum TextOption {
    FPS,
    CAMERA_POSITION,
    CAMERA_ORIENTATION,
    CAMERA_TIME,
    CAMERA_ANGLE
};

enum TextAlignment {
    LEFT_ALIGNMENT,
    //CENTER_ALIGNMENT,
    //RIGHT_ALIGNMENT
};

class GUI {
private:
    struct Character {
        GLuint     texture_id;
        glm::ivec2 size;
        glm::ivec2 bearing;
        GLuint     advance;
    };
    
    struct DisplayedInfo {
        std::string text;
        std::string variable;
        glm::vec2 position;
        glm::vec3 color;
        TextAlignment alignment;
        //float length;
        DisplayedInfo() {}
        DisplayedInfo(const std::string& text, const glm::vec2& position, const glm::vec3& color, TextAlignment alignment) : text(text), position(position), color(color), alignment(alignment) { variable = ""; }
    };
    
    Shader font_shader;
    
    std::map<GLchar, Character> characters;
    GLuint VAO, VBO;
    
    glm::mat4 projection;
    float scr_width;
    
    DisplayedInfo info[5];
    
    /*void findTextWidth(DisplayedInfo& inf) {
        inf.length = float(characters[97].bearing.x + (characters[97].advance >> 6)) * (float(inf.text.length() + inf.variable.length())) * 2.0f;
    }*/
    
    void setText() {
        info[FPS] = DisplayedInfo("FPS", glm::vec2(20, 20), glm::vec3(0, 1, 0), LEFT_ALIGNMENT);
        //findTextWidth(info[FPS]);
        info[CAMERA_POSITION] = DisplayedInfo("Position", glm::vec2(20, 75), glm::vec3(0, 1, 0), LEFT_ALIGNMENT);
        //findTextWidth(info[CAMERA_POSITION]);
        info[CAMERA_ORIENTATION] = DisplayedInfo("Orientation", glm::vec2(20, 130), glm::vec3(0, 1, 0), LEFT_ALIGNMENT);
        //findTextWidth(info[CAMERA_ORIENTATION]);
        info[CAMERA_TIME] = DisplayedInfo("Time", glm::vec2(20, 185), glm::vec3(0, 1, 0), LEFT_ALIGNMENT);
        //findTextWidth(info[CAMERA_TIME]);
        info[CAMERA_ANGLE] = DisplayedInfo("Angle", glm::vec2(20, 240), glm::vec3(0, 1, 0), LEFT_ALIGNMENT);
        //findTextWidth(info[CAMERA_ANGLE]);
    }
    
public:
    // REMEMBER TO CALL "loadFont" BEFORE USING THE OBJECT
    GUI(float width, float height) : projection(glm::ortho(0.0f, width, 0.0f, height)) {
        scr_width = width;
    }
    
    void updateText(TextOption text_option, const std::string& text) {
        info[text_option].variable = text;
        //findTextWidth(info[text_option]);
    }
    
    void draw() {
        glClear(GL_DEPTH_BUFFER_BIT);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        font_shader.use();
        
        for(int i = 0; i < 5; i++) {
            //if(info[i].alignment == LEFT_ALIGNMENT)
                renderText(info[i].text + ": " + info[i].variable, info[i].position.x, info[i].position.y, 1.0f, info[i].color);
            /*else
                renderText(info[i].text + ": " + info[i].variable, scr_width - info[i].length - info[i].position.x, info[i].position.y, 1.0f, info[i].color);*/
        }
        
        glDisable(GL_BLEND);
    }
    
    void loadFont(const char* font_path, int size) {
        FT_Library ft;
        FT_Face face;
        
        if (FT_Init_FreeType(&ft)) std::cout << "ERROR: Could not load FreeType library" << std::endl;
        
        if (FT_New_Face(ft, font_path, 0, &face)) std::cout << "ERROR: Failed to load font" << std::endl;
        
        FT_Set_Pixel_Sizes(face, 0, size);
        
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
          
        for (GLubyte c = 0; c < 128; c++) {
            
            if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
                std::cout << "ERROR: Failed to load a glyph in FreeType" << std::endl;
                continue;
            }
            
            GLuint texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                face->glyph->bitmap.width,
                face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer
            );
            
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            
            Character character = {
                texture,
                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                GLuint(face->glyph->advance.x)
            };
            characters.insert(std::pair<GLchar, Character>(c, character));
        }
        
        
        FT_Done_Face(face);
        FT_Done_FreeType(ft);
        
        
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        
        font_shader = Shader("src/shaders/font/font.vs", "src/shaders/font/font.fs");
        
        font_shader.use();
        font_shader.setMat4("projection", projection);
        
        setText();
    }
    
    void renderText(const std::string& text, GLfloat x, GLfloat y, GLfloat scale, const glm::vec3& text_color) {
        font_shader.setVec3("text_color", text_color);
        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(VAO);

        std::string::const_iterator c;
        for (c = text.begin(); c != text.end(); c++) {
            Character ch = characters[*c];

            GLfloat x_pos = x + ch.bearing.x * scale;
            GLfloat y_pos = y - (ch.size.y - ch.bearing.y) * scale;

            GLfloat w = ch.size.x * scale;
            GLfloat h = ch.size.y * scale;
            // Update VBO for each character
            GLfloat vertices[6][4] = {
                { x_pos,     y_pos + h,   0.0, 0.0 },
                { x_pos,     y_pos,       0.0, 1.0 },
                { x_pos + w, y_pos,       1.0, 1.0 },

                { x_pos,     y_pos + h,   0.0, 0.0 },
                { x_pos + w, y_pos,       1.0, 1.0 },
                { x_pos + w, y_pos + h,   1.0, 0.0 }
            };
            glBindTexture(GL_TEXTURE_2D, ch.texture_id);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            
            glDrawArrays(GL_TRIANGLES, 0, 6);
            
            x += (ch.advance >> 6) * scale;
        }
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    
    void resize(float width, float height) {
        projection = glm::ortho(0.0f, width, 0.0f, height);
        font_shader.setMat4("projection", projection);
        scr_width = width;
    }
};

#endif /* gui_h */
