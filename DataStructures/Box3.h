
#ifndef __BOX3
#define __BOX3

#include <Eigen\Core>
#include <Eigen\Geometry>

using namespace std;
using namespace Eigen;

class MyBox3 {
public:

    /// min coordinate point
    Eigen::Vector3d min;
    /// max coordinate point
    Eigen::Vector3d max;
        /// The bounding box constructor
    inline  MyBox3() { min.x()= 1;max.x()= -1;min.y()= 1;max.y()= -1;min.z()= 1;max.z()= -1;}
        /// Copy constructor
    inline  MyBox3( const MyBox3 & b ) { min=b.min; max=b.max; }
        /// Min Max constructor
    inline  MyBox3( const Eigen::Vector3d & mi, const Eigen::Vector3d & ma ) { min = mi; max = ma; }
    /// Point Radius Constructor
  inline MyBox3(const Eigen::Vector3d & center, const double & radius) {
    min = center-Eigen::Vector3d(radius,radius,radius);
    max = center+Eigen::Vector3d(radius,radius,radius);
  }
        /// The bounding box distructor
    inline ~MyBox3() { }
        /// Operator to compare two bounding box
    inline bool operator == ( MyBox3 const & p ) const
    {
        return min==p.min && max==p.max;
    }
        /// Operator to dispare two bounding box
    inline bool operator != ( MyBox3 const & p ) const
    {
        return min!=p.min || max!=p.max;
    }
        /** Varia le dimensioni del bounding box scalandole rispetto al parametro scalare.
            @param s Valore scalare che indica di quanto deve variare il bounding box
        */
    void Offset( const double s )
    {
        Offset( Eigen::Vector3d (s,s,s));
    }
        /** Varia le dimensioni del bounding box del valore fornito attraverso il parametro.
            @param delta Point in 3D space
        */
    void Offset( const Eigen::Vector3d & delta )
    {
        min -= delta;
        max += delta;
    }
        /// Initializing the bounding box
    void Set( const Eigen::Vector3d & p )
    {
        min = max = p;
    }
        /// Set the bounding box to a null value
    void SetNull()
    {
        min.x()= 1; max.x()= -1;
        min.y()= 1; max.y()= -1;
        min.z()= 1; max.z()= -1;
    }
        /** Function to add two bounding box
            @param b Il bounding box che si vuole aggiungere
        */
    void Add( MyBox3 const & b )
    {
        if(b.IsNull()) return; // Adding a null bbox should do nothing
        if(IsNull()) *this=b;
        else
        {
            if(min.x() > b.min.x()) min.x() = b.min.x();
            if(min.y() > b.min.y()) min.y() = b.min.y();
            if(min.z() > b.min.z()) min.z() = b.min.z();

            if(max.x() < b.max.x()) max.x() = b.max.x();
            if(max.y() < b.max.y()) max.y() = b.max.y();
            if(max.z() < b.max.z()) max.z() = b.max.z();
        }
    }
        /** Funzione per aggiungere un punto al bounding box. Il bounding box viene modificato se il punto
            cade fuori da esso.
            @param p The point 3D
        */
    void Add( const Eigen::Vector3d & p )
    {
        if(IsNull()) Set(p);
        else
        {
            if(min.x() > p.x()) min.x() = p.x();
            if(min.y() > p.y()) min.y() = p.y();
            if(min.z() > p.z()) min.z() = p.z();

            if(max.x() < p.x()) max.x() = p.x();
            if(max.y() < p.y()) max.y() = p.y();
            if(max.z() < p.z()) max.z() = p.z();
        }
    }

    /** Function to add a sphere (a point + radius) to a bbox
        @param p The point 3D
        @param radius the radius of the sphere centered on p
    */
void Add( const Eigen::Vector3d & p, const double radius )
{
    if(IsNull()) Set(p);
    else
    {
      min.x() = std::min(min.x(),p.x()-radius);
      min.y() = std::min(min.y(),p.y()-radius);
      min.z() = std::min(min.z(),p.z()-radius);

      max.x() = std::max(max.x(),p.x()+radius);
      max.y() = std::max(max.y(),p.y()+radius);
      max.z() = std::max(max.z(),p.z()+radius);
    }
}
    // Aggiunge ad un box un altro box trasformato secondo la matrice m
    void Add( const Eigen::Matrix3d &m, const MyBox3 & b )
    {
            const Eigen::Vector3d &mn= b.min;
            const Eigen::Vector3d &mx= b.max;
			Add(m*(Eigen::Vector3d(mn[0],mn[1],mn[2])));
            Add(m*(Eigen::Vector3d(mx[0],mn[1],mn[2])));
            Add(m*(Eigen::Vector3d(mn[0],mx[1],mn[2])));
            Add(m*(Eigen::Vector3d(mx[0],mx[1],mn[2])));
            Add(m*(Eigen::Vector3d(mn[0],mn[1],mx[2])));
            Add(m*(Eigen::Vector3d(mx[0],mn[1],mx[2])));
            Add(m*(Eigen::Vector3d(mn[0],mx[1],mx[2])));
            Add(m*(Eigen::Vector3d(mx[0],mx[1],mx[2])));
    }
        /** Calcola l'intersezione tra due bounding box. Al bounding box viene assegnato il valore risultante.
            @param b Il bounding box con il quale si vuole effettuare l'intersezione
        */
    void Intersect( const MyBox3 & b )
    {
        if(min.x() < b.min.x()) min.x() = b.min.x();
        if(min.y() < b.min.y()) min.y() = b.min.y();
        if(min.z() < b.min.z()) min.z() = b.min.z();

        if(max.x() > b.max.x()) max.x() = b.max.x();
        if(max.y() > b.max.y()) max.y() = b.max.y();
        if(max.z() > b.max.z()) max.z() = b.max.z();

        if(min.x()>max.x() || min.y()>max.y() || min.z()>max.z()) SetNull();
    }
        /** Trasla il bounding box di un valore definito dal parametro.
            @param p Il bounding box trasla sulla x e sulla y in base alle coordinate del parametro
        */
    void Translate( const Eigen::Vector3d & p )
    {
        min += p;
        max += p;
    }
        /** Verifica se un punto appartiene ad un bounding box.
            @param p The point 3D
            @return True se p appartiene al bounding box, false altrimenti
        */
    bool IsIn( Eigen::Vector3d const & p ) const
    {
        return (
            min.x() <= p.x() && p.x() <= max.x() &&
            min.y() <= p.y() && p.y() <= max.y() &&
            min.z() <= p.z() && p.z() <= max.z()
        );
    }
        /** Verifica se un punto appartiene ad un bounding box aperto sul max.
            @param p The point 3D
            @return True se p appartiene al bounding box, false altrimenti
        */
    bool IsInEx( Eigen::Vector3d const & p ) const {
        return (
            min.x() <= p.x() && p.x() < max.x() &&
            min.y() <= p.y() && p.y() < max.y() &&
            min.z() <= p.z() && p.z() < max.z()
        );
    }

    bool Collide(MyBox3 const &b) const {
        return b.min.x()<max.x() && b.max.x()>min.x() &&
               b.min.y()<max.y() && b.max.y()>min.y() &&
               b.min.z()<max.z() && b.max.z()>min.z() ;
    }


    bool IsNull() const { return min.x()>max.x() || min.y()>max.y() || min.z()>max.z(); }


    bool IsEmpty() const { return min==max; }

	double Diag() const {
        return (max-min).norm();
    }
    double SquaredDiag() const {
		return (max-min).squaredNorm();
    }
        /// Calcola il centro del bounding box.
    Eigen::Vector3d Center() const {
        return (min+max)/2;
    }
        /// Compute bounding box size.
    Eigen::Vector3d Dim() const {
        return (max-min);
    }
      /// Returns global coords of a local point expressed in [0..1]^3
    Eigen::Vector3d LocalToGlobal(Eigen::Vector3d const & p) const{
        return Eigen::Vector3d(
            min[0] + p[0]*(max[0]-min[0]),
            min[1] + p[1]*(max[1]-min[1]),
            min[2] + p[2]*(max[2]-min[2]));
    }
      /// Returns local coords expressed in [0..1]^3 of a point in 3D
    Eigen::Vector3d GlobalToLocal(Eigen::Vector3d const & p) const{
        return Eigen::Vector3d(
          (p[0]-min[0])/(max[0]-min[0]),
          (p[1]-min[1])/(max[1]-min[1]),
          (p[2]-min[2])/(max[2]-min[2])
            );
    }
        /// Calcola il volume del bounding box.
    double Volume() const
    {
        return (max.x()-min.x())*(max.y()-min.y())*(max.z()-min.z());
    }
        /// Calcola la dimensione del bounding box sulla x.
    inline double DimX() const { return max.x()-min.x();}
        /// Calcola la dimensione del bounding box sulla y.
    inline double DimY() const { return max.y()-min.y();}
        /// Calcola la dimensione del bounding box sulla z.
    inline double DimZ() const { return max.z()-min.z();}
        /// Calcola il lato di lunghezza maggiore
    inline unsigned char MaxDim() const {
        int i;
        Eigen::Vector3d diag = max-min;
        if(diag[0]>diag[1]) i=0; else i=1;
        return (diag[i]>diag[2])? i: 2;
    }
        /// Calcola il lato di lunghezza minore
    inline unsigned char MinDim() const {
        int i;
        Eigen::Vector3d diag =  max-min;
        if(diag[0]<diag[1]) i=0; else i=1;
        return (diag[i]<diag[2])? i: 2;
    }

    template <class Q>
    inline void Import( const MyBox3 & b )
    {
        min.Import(b.min);
        max.Import(b.max);
    }

   /* template <class Q>
    static inline Box3 Construct( const MyBox3 & b )
    {
    return Box3(Eigen::Vector3d::Construct(b.min),Eigen::Vector3d::Construct(b.max));
    }*/

        /// gives the ith box vertex in order: (x,y,z),(X,y,z),(x,Y,z),(X,Y,z),(x,y,Z),(X,y,Z),(x,Y,Z),(X,Y,Z)
    Eigen::Vector3d P(const int & i) const {
            return Eigen::Vector3d(
                min[0]+ (i%2) * DimX(),
                min[1]+ ((i / 2)%2) * DimY(),
                min[2]+ (i>3)* DimZ());
    }
}; // end class definition

/*template <class T> MyBox3<T> Eigen::Vector<T,3>::GetBBox(MyBox3<T> &bb) const {
 bb.Set( *this );
}*/

#endif

