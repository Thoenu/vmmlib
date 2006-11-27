/* 
* VMMLib - Vector & Matrix Math Lib
*  
* @author Stefan Eilemann
*
* @license BSD license, check LICENSE
*/ 

#ifndef _vmml_Frustum_H_
#define _vmml_Frustum_H_


// - declaration -

namespace vmml
{

template< class T > 
class Frustum
{
public:
    union
    {
        struct
        {
            T left;
            T right;
            T top;
            T bottom;
            T near;
            T far;
        };
        T data[6];
    };
            
    // contructors
    Frustum(); // warning: components NOT initialised ( for performance )
    Frustum( const T left, const T right, const T top, const T bottom,
             const T near, const T far );
        
    // dangerous, but implemented to allow easy conversion between 
    // Frustum< float > and Frustum< double >
    //the pointer 'values' must be a valid 6 component c array of the resp. type
    Frustum( const float* values );
    Frustum( const double* values );

    ~Frustum();

    Matrix4<T> computeMatrix() const;
    void       adjustNear( const T near );

    friend std::ostream& operator << ( std::ostream& os, const Frustum& frustum)
    {
        const std::ios::fmtflags flags = os.flags();
        const int                prec  = os.precision();

        os.setf( std::ios::right, std::ios::adjustfield );
        os.precision( 5 );
        os << "[" << std::setw(10) << frustum.left << " " 
           << std::setw(10) << frustum.right  << " " 
           << std::setw(10) << frustum.top    << " " 
           << std::setw(10) << frustum.bottom << " " 
           << std::setw(10) << frustum.near   << " " 
           << std::setw(10) << frustum.far    << "]";
        os.precision( prec );
        os.setf( flags );
        return os;
    };  

    static const Frustum DEFAULT;
};

#ifndef VMMLIB_DISABLE_TYPEDEFS
    typedef Frustum<float>  Frustumf;
    typedef Frustum<double> Frustumd;
#endif
}

// - implementation - //
#include "Matrix4.h"

namespace vmml
{

template< typename T > 
const Frustum< T > Frustum< T >::DEFAULT( -1.0, 1.0, -1.0, 1.0, 0.1, 100.0 );
       
template < class T > 
Frustum< T >::Frustum() 
{} 

template < class T > 
Frustum<T>::Frustum( const T _left, const T _right, const T _top, 
                     const T _bottom, const T _near, const T _far )
    : left( _left ),
      right( _right ),
      top( _top ),
      bottom( _bottom ),
      near( _near ),
      far( _far )
{} 

template < class T > 
Frustum< T >::Frustum( const float* values )
{
    assert( values && "Frustum: Nullpointer argument as source for initialisation!" );
    left   = static_cast< T > ( values[0] );
    right  = static_cast< T > ( values[1] );
    top    = static_cast< T > ( values[2] );
    bottom = static_cast< T > ( values[3] );
    near   = static_cast< T > ( values[4] );
    far    = static_cast< T > ( values[5] );
}

template < class T > 
Frustum< T >::Frustum( const double* values )
{
    assert( values && "Frustum: Nullpointer argument as source for initialisation!" );
    left   = static_cast< T > ( values[0] );
    right  = static_cast< T > ( values[1] );
    top    = static_cast< T > ( values[2] );
    bottom = static_cast< T > ( values[3] );
    near   = static_cast< T > ( values[4] );
    far    = static_cast< T > ( values[5] );
}


template < class T > 
Frustum< T >::~Frustum()
{}

template < class T > 
void Frustum<T>::adjustNear( const T _near )
{
    if( _near == near )
        return;

    const T ratio_2 = 0.5f * _near / near;

    const T hMiddle = (right + left) * 0.5f;
    const T width_2 = (right - left) * ratio_2;
    right = hMiddle + width_2;
    left  = hMiddle - width_2; 

    const T vMiddle  = (top + bottom) * 0.5f;
    const T height_2 = (top - bottom) * ratio_2;
    top    = vMiddle + height_2;
    bottom = vMiddle - height_2;

    near = _near;
}

template < class T > 
Matrix4<T> Frustum<T>::computeMatrix() const
{
    Matrix4<T> matrix = Matrix4<T>::IDENTITY;

    matrix.m00 = 2 * near / (right - left);
    matrix.m02 = (right + left) / (right - left );
    
    matrix.m11 = 2 * near / (top - bottom);
    matrix.m12 = (top + bottom) / (top - bottom);

    matrix.m22 = (far + near) / (far - near);
    matrix.m23 = 2 * far * near / (far - near);

    matrix.m32 = -1;
    matrix.m33 =  0;

    return matrix;
}

}	
#endif