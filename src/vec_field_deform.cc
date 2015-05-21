#include "vec_field_deform.h"

#include <zjucad/matrix/itr_matrix.h>
#include <jtflib/mesh/io.h>

using namespace std;
using namespace zjucad::matrix;
using namespace Eigen;

namespace geom_deform {

double eval_blend_val(const double r, const double ri, const double ro) {
    return 4*std::pow(r-ri, 3)*(1-(r-ri)/(ro-ri))/std::pow(ro-ri, 3) + std::pow(r-ri, 4)/std::pow(ro-ri, 4);
}

double eval_blend_jac(const double r, const double ri, const double ro) {
    return 12*std::pow(r-ri, 2)*(1-(r-ri)/(ro-ri))/std::pow(ro-ri, 3);
}

int get_ortn_basis(const double *v, double *u, double *w) {
    if ( v[0]*v[0] + v[1]*v[1] + v[2]*v[2] <= 1e-16 ) {
        cerr << "# zero direction\n";
        return 1;
    }
    Vector3d V(v[0], v[1], v[2]);
    Vector3d U, W;
    U.setZero();
    W.setZero();
    if ( V[0]*V[0] + V[1]*V[1] <= 1e-16 ) {
        U[0] = 1.0;
    } else {
        U[0] = -V[1];
        U[1] = V[0];
        U /= U.norm();
    }
    W = V.cross(U);
    W /= W.norm();
    std::copy(U.data(), U.data()+3, u);
    std::copy(W.data(), W.data()+3, w);
    return 0;
}

vel_field_deform::vel_field_deform() {
    advect_ = std::make_shared<advector>(vf_);
}

int vel_field_deform::load_model(const char *file) {
    matrix<size_t> cell;
    matrix<double> nods;
    int state = jtf::mesh::load_obj(file, cell, nods);
    cell_.resize(cell.size(1), cell.size(2));
    nods_.resize(nods.size(1), nods.size(2));
    std::copy(cell.begin(), cell.end(), cell_.data());
    std::copy(nods.begin(), nods.end(), nods_.data());
    return state;
}

int vel_field_deform::translate_deform(const Vec3 &src, const Vec3 &des, const double ri, const double ro) {
    const double len = (des-src).norm();
    const Vec3 dir = (des-src) / len;
    for (size_t i = 0; i < 100; ++i) {
        Vec3 c = src + i/100.0*len*dir;
        vf_.push_back(std::make_shared<vector_field>(c, ri, ro, dir));
#pragma omp parallel for
        for (size_t id = 0; id < nods_.cols(); ++id) {
            nods_.col(id) += (*advect_)(nods_.col(id));
        }
    }
    return 0;
}

int vel_field_deform::save_model(const char *file) {
    matrix<size_t> cell(cell_.rows(), cell_.cols());
    matrix<double> nods(nods_.rows(), nods_.cols());
    std::copy(cell_.data(), cell_.data()+cell_.size(), cell.begin());
    std::copy(nods_.data(), nods_.data()+nods_.size(), nods.begin());
    int state = jtf::mesh::save_obj(file, cell, nods);
    return state;
}

}