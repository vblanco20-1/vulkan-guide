// This code is in the public domain -- castanyo@yahoo.es

#ifndef NV_MATH_SPHERICALHARMONIC_H
#define NV_MATH_SPHERICALHARMONIC_H

#include "nvmath.h"
#include "Vector.h"

#include <string.h> // memcpy


namespace nv
{
    class Vector3;
    class Matrix;

    float legendrePolynomial( int l, int m, float x ) NV_CONST;
    float shBasis( int l, int m, float theta, float phi ) NV_CONST;
    float shBasis( int l, int m, const Vector3 & v ) NV_CONST;
    float hshBasis( int l, int m, float theta, float phi ) NV_CONST;
    float hshBasis( int l, int m, const Vector3 & v ) NV_CONST;

    class Sh;
    float dot(const Sh & a, const Sh & b) NV_CONST;


    /// Spherical harmonic class.
    class Sh
    {
        friend class Sh2;
        friend class ShMatrix;
    public:

        /// Construct a spherical harmonic of the given order.
        Sh(int o) : order(o)
        {
            coef = new float[basisNum()];
        }

        /// Copy constructor.
        Sh(const Sh & sh) : order(sh.order)
        {
            coef = new float[basisNum()];
            memcpy(coef, sh.coef, sizeof(float) * basisNum());
        }

        /// Destructor.
        ~Sh()
        {
            delete [] coef;
            coef = NULL;
        }

        /// Get number of bands.
        static int bandNum(int order) {
            return order + 1;
        }

        /// Get number of sh basis.
        static int basisNum(int order) {
            return (order + 1) * (order + 1);
        }

        /// Get the index for the given coefficients.
        static int index( int l, int m ) {
            return l * l + l + m;
        }

        /// Get sh order.
        int bandNum() const
        {
            return bandNum(order);
        }

        /// Get sh order.
        int basisNum() const
        {
            return basisNum(order);
        }

        /// Get sh coefficient indexed by l,m.
        float elem( int l, int m ) const
        {
            return coef[index(l, m)];
        }

        /// Get sh coefficient indexed by l,m.
        float & elem( int l, int m )
        {
            return coef[index(l, m)];
        }


        /// Get sh coefficient indexed by i.
        float elemAt( int i ) const {
            return coef[i];
        }

        /// Get sh coefficient indexed by i.
        float & elemAt( int i )
        {
            return coef[i];
        }


        /// Reset the sh coefficients.
        void reset()
        {
            for( int i = 0; i < basisNum(); i++ ) {
                coef[i] = 0.0f;
            }
        }

        /// Copy spherical harmonic.
        void operator= ( const Sh & sh )
        {
            nvDebugCheck(order <= sh.order);

            for(int i = 0; i < basisNum(); i++) {
                coef[i] = sh.coef[i];
            }
        }

        /// Add spherical harmonics.
        void operator+= ( const Sh & sh )
        {
            nvDebugCheck(order == sh.order);

            for(int i = 0; i < basisNum(); i++) {
                coef[i] += sh.coef[i];
            }
        }

        /// Substract spherical harmonics.
        void operator-= ( const Sh & sh )
        {
            nvDebugCheck(order == sh.order);

            for(int i = 0; i < basisNum(); i++) {
                coef[i] -= sh.coef[i];
            }
        }

        // Not exactly convolution, nor product.
        void operator*= ( const Sh & sh )
        {
            nvDebugCheck(order == sh.order);

            for(int i = 0; i < basisNum(); i++) {
                coef[i] *= sh.coef[i];
            }
        }

        /// Scale spherical harmonics.
        void operator*= ( float f )
        {
            for(int i = 0; i < basisNum(); i++) {
                coef[i] *= f;
            }
        }

        /// Add scaled spherical harmonics.
        void addScaled( const Sh & sh, float f )
        {
            nvDebugCheck(order == sh.order);

            for(int i = 0; i < basisNum(); i++) {
                coef[i] += sh.coef[i] * f;
            }
        }


        /*/// Add a weighted sample to the sh coefficients.
        void AddSample( const Vec3 & dir, const Color3f & color, float w=1.0f ) {
        for(int l = 0; l <= order; l++) {
        for(int m = -l; m <= l; m++) {
        Color3f & elem = GetElem(l, m);
        elem.Mad( elem, color, w * shBasis(l, m, dir) );
        }
        }
        }*/

        /// Evaluate 
        void eval(const Vector3 & dir)
        {
            for(int l = 0; l <= order; l++) {
                for(int m = -l; m <= l; m++) {
                    elem(l, m) = shBasis(l, m, dir);
                }
            }
        }


        /// Evaluate the spherical harmonic function.
        float sample(const Vector3 & dir) const
        {
            Sh sh(order);
            sh.eval(dir);

            return dot(sh, *this);
        }


        const int order;
        float * coef;

    };


    /// Compute dot product of the spherical harmonics.
    inline float dot(const Sh & a, const Sh & b)
    {
        nvDebugCheck(a.order == b.order);

        float sum = 0;
        for( int i = 0; i < Sh::basisNum(a.order); i++ ) {
            sum += a.elemAt(i) * b.elemAt(i);
        }

        return sum;
    }


    /// Second order spherical harmonic.
    class Sh2 : public Sh
    {
    public:

        /// Constructor.
        Sh2() : Sh(2) {}

        /// Copy constructor.
        Sh2(const Sh2 & sh) : Sh(sh) {}

        // Fast evaluation from: PPS' Efficient Spherical Harmonic Evaluation http://jcgt.org/published/0002/02/06/
        void eval(const Vector3 & dir) {
            float fZ2 = dir.z * dir.z;
            coef[0] = 0.2820947917738781f;
            coef[2] = 0.4886025119029199f * dir.z;
            coef[6] = 0.9461746957575601f * fZ2 + -0.3153915652525201f;

            float fC0 = dir.x;
            float fS0 = dir.y;

            float fTmpA = -0.48860251190292f;
            coef[3] = fTmpA * fC0;
            coef[1] = fTmpA * fS0;
            
            float fTmpB = -1.092548430592079f * dir.z;
            coef[7] = fTmpB * fC0;
            coef[5] = fTmpB * fS0;

            float fC1 = dir.x * fC0 - dir.y * fS0;
            float fS1 = dir.x * fS0 + dir.y * fC0;

            float fTmpC = 0.5462742152960395f;
            coef[8] = fTmpC * fC1;
            coef[4] = fTmpC * fS1;
        }

        /// Spherical harmonic resulting from projecting the clamped cosine transfer function to the SH basis.
        void cosineTransfer() {
            const float c1 = 0.282095f;	// K(0, 0)
            const float c2 = 0.488603f; // K(1, 0)
            const float c3 = 1.092548f; // sqrt(15.0f / PI) / 2.0f = K(2, -2)
            const float c4 = 0.315392f; // sqrt(5.0f / PI) / 4.0f) = K(2, 0)
            const float c5 = 0.546274f; // sqrt(15.0f / PI) / 4.0f) = K(2, 2)

            const float normalization = PI * 16.0f / 17.0f;

            const float const1 = c1 * normalization * 1.0f;
            const float const2 = c2 * normalization * (2.0f / 3.0f);
            const float const3 = c3 * normalization * (1.0f / 4.0f);
            const float const4 = c4 * normalization * (1.0f / 4.0f);
            const float const5 = c5 * normalization * (1.0f / 4.0f);

            coef[0] = const1;

            coef[1] = -const2;
            coef[2] = const2;
            coef[3] = -const2;

            coef[4] = const3;
            coef[5] = -const3;
            coef[6] = const4;
            coef[7] = -const3;
            coef[8] = const5;
        }
    };



    /// Spherical harmonic matrix.
    class ShMatrix
    {
    public:

        /// Create an identity matrix of the given order.
        ShMatrix(int o = 2) : m_order(o), m_identity(true)
        {
            nvCheck(m_order > 0);
            m_e = new float[size()];
            m_band = new float *[bandNum()];		
            setupBands();
        }

        /// Destroy and free matrix elements.
        ~ShMatrix()
        {
            delete m_e;
            delete m_band;
        }

        /// Set identity matrix.
        void setIdentity()
        {
            m_identity = true;
        }

        /// Return true if this is an identity matrix, false in other case.
        bool isIdentity() const {
            return m_identity;
        }

        /// Get number of bands of this matrix.
        int bandNum() const
        {
            return m_order+1;
        }

        /// Get total number of elements in the matrix.
        int size() const
        {
            int size = 0;
            for (int i = 0; i < bandNum(); i++) {
                size += square(i * 2 + 1);
            }
            return size;
        }

        /// Get element at the given raw index.
        float element(int idx) const
        {
            return m_e[idx];
        }

        /// Get element at the given with the given indices.
        float & element(int b, int x, int y)
        {
            nvDebugCheck(b >= 0);
            nvDebugCheck(b < bandNum());
            return m_band[b][(b + y) * (b * 2 + 1) + (b + x)];
        }

        /// Get element at the given with the given indices.
        float element(int b, int x, int y) const
        {
            nvDebugCheck(b >= 0);
            nvDebugCheck(b < bandNum());
            return m_band[b][(b + y) * (b * 2 + 1) + (b + x)];
        }

        /// Copy matrix.
        void copy(const ShMatrix & m)
        {
            nvDebugCheck(m_order == m.m_order);
            memcpy(m_e, m.m_e, size() * sizeof(float));
        }

        /// Rotate the given coefficients.
        /*void transform( const Sh & restrict source,  Sh * restrict dest ) const {
            nvCheck( &source != dest );	// Make sure there's no aliasing.
            nvCheck( dest->order <= order );
            nvCheck( order <= source.order );

            if (m_identity) {
                *dest = source;
                return;
            }

            // Loop through each band.
            for (int l = 0; l <= dest->order; l++) {

                for (int mo = -l; mo <= l; mo++) {

                    Color3f rgb = Color3f::Black;

                    for( int mi = -l; mi <= l; mi++ ) {
                        rgb.Mad( rgb, source.elem(l, mi), elem(l, mo, mi) );
                    }

                    dest->elem(l, mo) = rgb;
                }
            }
        }*/


        void multiply( const ShMatrix &A, const ShMatrix &B );
        void rotation( const Matrix & m );
        void rotation( int axis, float angles );
        void print();


    private:

        // @@ These could be static indices precomputed only once.
        /// Setup the band pointers.
        void setupBands()
        {
            int size = 0;
            for( int i = 0; i < bandNum(); i++ ) {
                m_band[i] = &m_e[size];
                size += square(i * 2 + 1);
            }
        }


    private:

        // Matrix order.
        const int m_order;

        // Identity flag for quick transform.
        bool m_identity;

        // Array of elements.
        float * m_e;

        // Band pointers.
        float ** m_band;

    };


} // nv namespace

#endif // NV_MATH_SPHERICALHARMONIC_H
