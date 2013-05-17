
#include "bloom_utils.h"

const Surface *currentSurface;

void createSurface(Surface *surface, GLboolean depth, GLboolean fp, GLboolean linear, GLboolean clampToEdge)
{
	surface->useDepth = depth;
	surface->useFp = fp;

    GLenum internalFormat = fp ? GL_RGBA16F_ARB : GL_RGBA;
    GLenum type = fp ? GL_HALF_FLOAT_ARB : GL_UNSIGNED_BYTE;
    GLenum filter = linear ? GL_LINEAR : GL_NEAREST;

    // create a color texture
	glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &surface->texture);
    glBindTexture(GL_TEXTURE_2D, surface->texture);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, surface->width, surface->height, 0, GL_RGBA, type, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
	if(clampToEdge){
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
    glBindTexture(GL_TEXTURE_2D, 0);

    // create depth renderbuffer
    if (surface->useDepth)
    {
		glActiveTexture(GL_TEXTURE0);
		glGenTextures(1, &surface->depth);
		glBindTexture(GL_TEXTURE_2D, surface->depth);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, surface->width, surface->height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
		if(clampToEdge){
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		}
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
		glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
		glBindTexture(GL_TEXTURE_2D, 0);
		/* // Decided to use texture instead of renderbuffer, so I could read in depth buffer and use it for effects
		//cout << "depth used\n";
        glGenRenderbuffers(1, &surface->depth);
        glBindRenderbuffer(GL_RENDERBUFFER, surface->depth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, surface->width, surface->height);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		*/
    }
    else
    {
		//cout << "depth not used\n";
        surface->depth = 0;
    }

    // create FBO itself
    glGenFramebuffers(1, &surface->fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, surface->fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, surface->texture, 0);
    if (surface->useDepth)
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, surface->depth, 0);
        //glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, surface->depth);
    GLenum status;
	if ((status = glCheckFramebufferStatus(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE) {
		fprintf(stderr, "glCheckFramebufferStatus: error %p", status);
		fprintf(stderr, "\n");
		return;
	}
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void bindSurface(Surface *surface)
{
    glBindFramebuffer(GL_FRAMEBUFFER, surface->fbo);
    glViewport(surface->viewport.x, surface->viewport.y, surface->viewport.width, surface->viewport.height);
    currentSurface = surface;
}

void clearSurfaceColor(GLfloat c0, GLfloat c1, GLfloat c2, GLfloat c3)
{
    const Surface *surface = currentSurface;
    glClearColor(c0, c1, c2, c3);
    glClear(GL_COLOR_BUFFER_BIT | (surface->useDepth ? GL_DEPTH_BUFFER_BIT : 0));
}

void resizeSurface(Surface *surface, int w, int h){
	//this funciton assumes viewport and texture sizes are the same, which breaks flexibility, but it works for our purposes

	surface->width = w;
	surface->height = h;
	surface->viewport.width = w;
	surface->viewport.height = h;

	glBindTexture(GL_TEXTURE_2D, surface->texture);
	GLenum internalFormat = surface->useFp ? GL_RGBA16F_ARB : GL_RGBA;
    GLenum type = surface->useFp ? GL_HALF_FLOAT_ARB : GL_UNSIGNED_BYTE;
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, surface->width, surface->height, 0, GL_RGBA, type, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
 

	if(surface->useDepth){
		glBindTexture(GL_TEXTURE_2D, surface->depth);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, surface->width, surface->height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}