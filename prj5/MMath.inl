/**
Jonathan Tatum
MMath.inl : inlined math functions 
*/

#ifndef MMATH_INL
#define MMATH_INL
/*==================================
Helpers
==================================*/

inline GLfloat degToRad(GLfloat deg){
	return(deg / 180.0f * PI);
}
inline GLfloat radToDeg(GLfloat rad){
	return (rad/PI*180.0f);
}

/*====================================
Quaternian definitions
====================================*/
	inline Quaternian::Quaternian(GLfloat x1,GLfloat y1,GLfloat z1,GLfloat w1):x(x1),y(y1),z(z1),w(w1){}

	inline Quaternian Quaternian::cross(Quaternian rhs){
		GLfloat wn,xn,yn,zn;
		wn = w*rhs.w - x*rhs.x - y*rhs.y - z*rhs.z;
		xn = w*rhs.x + x*rhs.w + y*rhs.z - z*rhs.y;
		yn = w*rhs.y - x*rhs.z + y*rhs.w + z*rhs.x; 
		zn = w*rhs.z + x*rhs.y - y*rhs.x + z*rhs.w;
		rhs.x = xn;
		rhs.y = yn;
		rhs.z = zn;
		rhs.w = wn;
		return (rhs); 
	}

	inline GLfloat Quaternian::dot(const Quaternian &rhs){
		GLfloat dp = w*rhs.w + x*rhs.x + y*rhs.y + z*rhs.z;
		return dp;
	}

	inline Quaternian& Quaternian::normalize(){
		GLfloat s = 1.0f/sqrt(dot(*this));
		scale(s);
		return *this;
	}

	/*
	angle specified in degrees
	*/
	inline Quaternian Quaternian::generateRotation(GLfloat angle, const Vec3f &axis){
		GLfloat alpha = degToRad(angle); 
		GLfloat s; 
		Vec3f v(axis); 
		v.normalize(); 
		s = sin(alpha/2);
		Quaternian result=Quaternian(v * s);
		result.w = cos(alpha / 2);
		return result;
	}

#define FD_THRESH (0.00001f) 
	inline Quaternian Quaternian::SLERP(Quaternian q1, Quaternian q2, GLfloat u){
		GLfloat alpha = acos(q1.dot(q2));
		GLfloat s1, s2; 
		s1 = (1-u); 
		s2 = (u); 
		if (alpha < 0){
			alpha = -alpha;
		}
		
		if (alpha < FD_THRESH){
			s1 = sin(s1*alpha)/sin(alpha);
			s2 = sin(s2*alpha)/sin(alpha); 
		}
		q2.scale(s2);
		q1.scale(s1); 
		return q2+=q1; 
	}
	
	inline Quaternian Quaternian::rot(const Vec3f& vector){
		return rot(Quaternian(vector)); 
	}

	inline Quaternian Quaternian::rot(const Quaternian &vec){
		return (cross(vec).cross(conj()));
	}

	inline Vec3f Quaternian::real(){
		return Vec3f(x, y, z); 
	}

	inline Quaternian& Quaternian::scale(GLfloat s){
		w *= s;
		z *= s;
		y *= s;
		x *= s; 
		return *this;
	}

	inline Quaternian& Quaternian::operator+=(const Quaternian& r){
		w += r.w;
		x += r.x;
		y += r.y;
		z += r.z;
		return *this;
 	}

	inline Quaternian operator+(Quaternian l, const Quaternian &r){
		l+=r; 
		return l; 
	}
/*============================
Matrix Functions
==============================*/

inline GLfloat* Matrix4::operator[](int idx){
	return data[idx];
}

inline Matrix4::Matrix4(const GLfloat *buffer){
	for (int i = 0; i < 16; i++){
		data[i%4][i/4] = buffer[i];  
	}
}

inline Matrix4 operator*(Matrix4 lhs,const Matrix4 &rhs){
		lhs *= rhs;
		return lhs; 
}

inline Vec4f operator*(const Matrix4 &lhs, Vec4f rhs){
	GLfloat d[4] = {0,0,0,0};
	for (int i = 0; i < 4; i++){	
		d[i] += lhs.data[0][i] * rhs.x + lhs.data[1][i] * rhs.y
		+ lhs.data[2][i] * rhs.z + lhs.data[3][i] * rhs.w; 
	}
	rhs.x = d[0];
	rhs.y = d[1];
	rhs.z = d[2];
	rhs.w = d[3]; 
	return rhs;
}

#endif
