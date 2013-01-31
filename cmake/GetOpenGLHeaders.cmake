set(EXT_HEADER_DIR ${CMAKE_SOURCE_DIR}/ext/include)

set(GL_HEADER_DIR ${EXT_HEADER_DIR}/GL)

#check for opengl3 headers
function(download_gl_header GL_HEADER)
    if(NOT EXISTS ${GL_HEADER_DIR}/${GL_HEADER})
        execute_process(
            WORKING_DIRECTORY ${GL_HEADER_DIR}
            COMMAND wget http://www.opengl.org/registry/api/${GL_HEADER}
    )
    endif()
endfunction(download_gl_header)

function(check_gl_headers)
  if(EXISTS /usr/include/GL/glcorearb.h AND NOT KORE_PLATTFORM_WINDOWS)
	  message("Core Profile API Header found in /usr/include")
  else()
	  message("No Core Profile API Header found. Using local ones.")
	  if(NOT EXISTS ${GL_HEADER_DIR})
		  execute_process(COMMAND mkdir -p ${GL_HEADER_DIR})
	  endif()
	  set(GL_HEADERS glcorearb.h glext.h glxext.h wglext.h)
	  foreach(GL_HEADER ${GL_HEADERS})
		      download_gl_header(${GL_HEADER})
    endforeach(GL_HEADER)

	  include_directories(${EXT_HEADER_DIR})
  endif()
endfunction(check_gl_headers)
