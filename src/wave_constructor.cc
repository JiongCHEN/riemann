#include "wave_constructor.h"

#include <iostream>
#include <zjucad/matrix/itr_matrix.h>
#include <zjucad/matrix/io.h>
#include <jtflib/mesh/io.h>

#include "def.h"
#include "write_vtk.h"
#include "config.h"

using namespace std;
using namespace zjucad::matrix;
using namespace Eigen;
using namespace jtf::mesh;
using mati_t = zjucad::matrix::matrix<size_t>;
using matd_t = zjucad::matrix::matrix<double>;

namespace riemann {

extern "C" {

void wave_value_condition_(double *val, const double *f, const double *c);
void wave_value_condition_jac_(double *jac, const double *f, const double *c);
void wave_value_condition_hes_(double *hes, const double *f, const double *c);

void modulus_condition_(double *val, const double *f);
void modulus_condition_jac_(double *jac, const double *f);
void modulus_condition_hes_(double *hes, const double *f);

void phase_condition_(double *val, const double *f);
void phase_condition_jac_(double *jac, const double *f);
void phase_condition_hes_(double *hes, const double *f);

}

//class wave_value_energy : public Functional<double>
//{
//public:
//  wave_value_energy(const mati_t &edge, const matd_t &f, const matd_t &c, const double w=1.0)
//    : edge_(edge), dim_(f.size()), c_(c), w_(w) {}
//  size_t Nx() const {
//    return dim_;
//  }
//  int Val(const double *x, double *val) const {
//    itr_matrix<const double *> X(4, dim_/4, x);
//    for (size_t i = 0; i < edge_.size(2); ++i) {
//      matd_t vert = X(colon(), edge_(colon(), i));
//      double value = 0;
//      wave_value_condition_(&value, &vert[0], &c_(0, i));
//      *val += w_*value;
//    }
//    return 0;
//  }
//  int Gra(const double *x, double *gra) const {
//    itr_matrix<const double *> X(4, dim_/4, x);
//    itr_matrix<double *> G(4, dim_/4, gra);
//    for (size_t i = 0; i <edge_.size(2); ++i) {
//      matd_t vert = X(colon(), edge_(colon(), i));
//      matd_t g = zeros<double>(4, 2);
//      wave_value_condition_jac_(&g[0], &vert[0], &c_(0, i));
//      G(colon(), edge_(colon(), i)) += w_*g;
//    }
//    return 0;
//  }
//  int Hes(const double *x, vector<Triplet<double>> *hes) const {
//    itr_matrix<const double *> X(4, dim_/4, x);
//    for (size_t i = 0; i < edge_.size(2); ++i) {
//      matd_t vert = X(colon(), edge_(colon(), i));
//      matd_t H = zeros<double>(8, 8);
//      wave_value_condition_hes_(&H[0], &vert[0], &c_(0, i));
//      for (size_t q = 0; q < H.size(2); ++q) {
//        for (size_t p = 0; p < H.size(1); ++p) {
//          if ( H(p, q) != 0.0 ) {
//            size_t I = 4*edge_(p/4, i)+p%4;
//            size_t J = 4*edge_(q/4, i)+q%4;
//            hes->push_back(Triplet<double>(I, J, w_*H(p, q)));
//          }
//        }
//      }
//    }
//    return 0;
//  }
//private:
//  const mati_t &edge_;
//  const matd_t &c_;
//  const size_t dim_;
//  const double w_;
//};

//class modulus_energy : public Functional<double>
//{
//public:
//  modulus_energy(const matd_t &f, const double w=0.15)
//    : dim_(f.size()), w_(w) {}
//  size_t Nx() const {
//    return dim_;
//  }
//  int Val(const double *x, double *val) const {
//    itr_matrix<const double *> X(4, dim_/4, x);
//    for (size_t i = 0; i < X.size(2); ++i) {
//      double value = 0;
//      modulus_condition_(&value, &X(0, i));
//      *val += w_*value;
//    }
//    return 0;
//  }
//  int Gra(const double *x, double *gra) const {
//    itr_matrix<const double *> X(4, dim_/4, x);
//    itr_matrix<double *> G(4, dim_/4, gra);
//#pragma omp parallel for
//    for (size_t i = 0; i < X.size(2); ++i) {
//      matd_t g  = zeros<double>(4, 1);
//      modulus_condition_jac_(&g[0], &X(0, i));
//      G(colon(), i) += w_*g;
//    }
//    return 0;
//  }
//  int Hes(const double *x, vector<Triplet<double>> *hes) const {
//    itr_matrix<const double *> X(4, dim_/4, x);
//    for (size_t i = 0; i < X.size(2); ++i) {
//      matd_t H = zeros<double>(4, 4);
//      modulus_condition_hes_(&H[0], &X(0, i));
//      for (size_t q = 0; q < H.size(2); ++q) {
//        for (size_t p = 0; p < H.size(1); ++p)  {
//          if ( H(p, q) != 0.0 )
//            hes->push_back(Triplet<double>(4*i+p, 4*i+q, w_*H(p, q)));
//        }
//      }
//    }
//    return 0;
//  }
//private:
//  const size_t dim_;
//  const double w_;
//};

//class phase_energy : public Functional<double>
//{
//public:
//  phase_energy(const matd_t &f, const double w=0.15)
//    : dim_(f.size()), w_(w) {}
//  size_t Nx() const {
//    return dim_;
//  }
//  int Val(const double *x, double *val) const {
//    itr_matrix<const double *> X(4, dim_/4, x);
//    for (size_t i = 0; i < X.size(2); ++i) {
//      double value = 0;
//      phase_condition_(&value, &X(0, i));
//      *val += w_*value;
//    }
//    return 0;
//  }
//  int Gra(const double *x, double *gra) const {
//    itr_matrix<const double *> X(4, dim_/4, x);
//    itr_matrix<double *> G(4, dim_/4, gra);
//#pragma omp parallel for
//    for (size_t i = 0; i < X.size(2); ++i) {
//      matd_t g  = zeros<double>(4, 1);
//      phase_condition_jac_(&g[0], &X(0, i));
//      G(colon(), i) += w_*g;
//    }
//    return 0;
//  }
//  int Hes(const double *x, vector<Triplet<double>> *hes) const {
//    itr_matrix<const double *> X(4, dim_/4, x);
//    for (size_t i = 0; i < X.size(2); ++i) {
//      matd_t H = zeros<double>(4, 4);
//      phase_condition_hes_(&H[0], &X(0, i));
//      for (size_t q = 0; q < H.size(2); ++q) {
//        for (size_t p = 0; p < H.size(1); ++p)  {
//          if ( H(p, q) != 0.0 )
//            hes->push_back(Triplet<double>(4*i+p, 4*i+q, w_*H(p, q)));
//        }
//      }
//    }
//    return 0;
//  }
//private:
//  const size_t dim_;
//  const double w_;
//};

class wave_value_cons : public Constraint<double>
{
public:
  wave_value_cons(const mati_t &edge, const matd_t &f, const matd_t &cIJ, const matd_t &cJI, const double w=1.0)
    : edge_(edge), cIJ_(cIJ), cJI_(cJI), nx_(f.size()), nf_(2*edge.size(2)), w_(sqrt(w)) {}
  size_t Nx() const {
    return nx_;
  }
  size_t Nf() const {
    return nf_;
  }
  int Val(const double *x, double *val) const {
    itr_matrix<const double *> X(4, nx_/4, x);
#pragma omp parallel for
    for (size_t i = 0; i < edge_.size(2); ++i) {
      matd_t vert = X(colon(), edge_(colon(), i));
      val[2*i+0] += w_*(vert(0, 1)-dot(cIJ_(colon(), i), vert(colon(), 0)));
      val[2*i+1] += w_*(vert(0, 0)-dot(cJI_(colon(), i), vert(colon(), 1)));
    }
    return 0;
  }
  int Jac(const double *x, const size_t off, vector<Triplet<double>> *jac) const {
    itr_matrix<const double *> X(4, nx_/4, x);
    for (size_t i = 0; i < edge_.size(2); ++i) {
      // I->J
      jac->push_back(Triplet<double>(off+2*i+0, 4*edge_(1, i)+0, w_*1.0));
      jac->push_back(Triplet<double>(off+2*i+0, 4*edge_(0, i)+0, -w_*cIJ_(0, i)));
      jac->push_back(Triplet<double>(off+2*i+0, 4*edge_(0, i)+1, -w_*cIJ_(1, i)));
      jac->push_back(Triplet<double>(off+2*i+0, 4*edge_(0, i)+2, -w_*cIJ_(2, i)));
      jac->push_back(Triplet<double>(off+2*i+0, 4*edge_(0, i)+3, -w_*cIJ_(3, i)));
      // J->I
//      jac->push_back(Triplet<double>(off+2*i+1, 4*edge_(0, i)+0, w_*1.0));
//      jac->push_back(Triplet<double>(off+2*i+1,));
//      jac->push_back(Triplet<double>(off+2*i+1,));
//      jac->push_back(Triplet<double>(off+2*i+1,));
//      jac->push_back(Triplet<double>(off+2*i+1, ));
    }
    return 0;
  }
private:
  const mati_t &edge_;
  const matd_t &cIJ_, &cJI_;
  const size_t nx_, nf_;
  const double w_;
};

class modulus_cons : public Constraint<double>
{
public:
  modulus_cons(const matd_t &f, const double w)
    : nx_(f.size()), nf_(f.size(2)), w_(sqrt(w)) {}
  size_t Nx() const {
    return nx_;
  }
  size_t Nf() const {
    return nf_;
  }
  int Val(const double *x, double *val) const {
    itr_matrix<const double *> X(4, nx_/4, x);
#pragma omp parallel for
    for (size_t i = 0; i < X.size(2); ++i) {
      matd_t vert = X(colon(), i);
      val[i] += w_*(dot(vert, vert)-1.0);
    }
    return 0;
  }
  int Jac(const double *x, const size_t off, vector<Triplet<double>> *jac) const {
    itr_matrix<const double *> X(4, nx_/4, x);
    for (size_t i = 0; i < X.size(2); ++i) {
      jac->push_back(Triplet<double>(off+i, 4*i+0, 2*w_*X(0, i)));
      jac->push_back(Triplet<double>(off+i, 4*i+1, 2*w_*X(1, i)));
      jac->push_back(Triplet<double>(off+i, 4*i+2, 2*w_*X(2, i)));
      jac->push_back(Triplet<double>(off+i, 4*i+3, 2*w_*X(3, i)));
    }
    return 0;
  }
private:
  const size_t nx_, nf_;
  const double w_;
};

class phase_cons : public Constraint<double>
{
public:
  phase_cons(const matd_t &f, const double w)
    : nx_(f.size()), nf_(f.size(2)), w_(sqrt(w)) {}
  size_t Nx() const {
    return nx_;
  }
  size_t Nf() const {
    return nf_;
  }
  int Val(const double *x, double *val) const {
    itr_matrix<const double *> X(4, nx_/4, x);
#pragma omp parallel for
    for (size_t i = 0; i < X.size(2); ++i) {
      val[i] += w_*(X(0, i)*X(3, i)-X(1, i)*X(2, i));
    }
    return 0;
  }
  int Jac(const double *x, const size_t off, vector<Triplet<double> > *jac) const {
    itr_matrix<const double *> X(4, nx_/4, x);
    for (size_t i = 0; i < X.size(2); ++i) {
      jac->push_back(Triplet<double>(off+i, 4*i+0, w_*X(3, i)));
      jac->push_back(Triplet<double>(off+i, 4*i+1, -w_*X(2, i)));
      jac->push_back(Triplet<double>(off+i, 4*i+2, -w_*X(1, i)));
      jac->push_back(Triplet<double>(off+i, 4*i+3, w_*X(0, i)));
    }
    return 0;
  }
private:
  const size_t nx_, nf_;
  const double w_;
};

class feature_cons : public Constraint<double>
{
public:
  feature_cons(const matd_t &f);
  size_t Nx() const;
  size_t Nf() const;
  int Val(const double *x, double *val) const;
  int Jac(const double *x, const size_t off, vector<Triplet<double>> *jac) const;
private:
};

//==============================================================================
int wave_constructor::load_model_from_obj(const char *filename) {
  return jtf::mesh::load_obj(filename, tris_, nods_);
}

int wave_constructor::load_frame_field(const char *filename) {
  e2c_.reset(edge2cell_adjacent::create(tris_, false));
  if ( !e2c_.get() )
    return __LINE__;
  ifstream ifs(filename);
  if ( ifs.fail() ) {
    cerr << "[error] can not open " << filename << endl;
    return __LINE__;
  }
  size_t edge_num;
  ifs >> edge_num;
  ASSERT(edge_num == e2c_->edges_.size());
  frame_field_.resize(6, edge_num);
  size_field_.resize(2, edge_num);
  size_t p, q, ide;
  for (size_t i = 0; i < edge_num; ++i) {
    ifs >> p >> q;
    ide = e2c_->get_edge_idx(p, q);
    ifs >> frame_field_(0, i) >> frame_field_(1, i) >> frame_field_(2, i)
        >> frame_field_(3, i) >> frame_field_(4, i) >> frame_field_(5, i);
    size_field_(0, i) = norm(frame_field_(colon(0, 2), i));
    size_field_(1, i) = norm(frame_field_(colon(3, 5), i));
    frame_field_(colon(0, 2), i) /= size_field_(0, i);
    frame_field_(colon(3, 5), i) /= size_field_(1, i);
  }
  return 0;
}

int wave_constructor::vis_edge_frame_field(const char *file_x, const char *file_y, const double scale) const {
  if ( edges_.size() == 0 ) {
    cerr << "[error] edge is not initialized\n";
    return __LINE__;
  }
  matd_t X = scale*frame_field_(colon(0, 2), colon());
  draw_edge_direct_field(file_x, &nods_[0], nods_.size(2), &edges_[0], edges_.size(2), &X[0]);
  matd_t Y = scale*frame_field_(colon(3, 5), colon());
  draw_edge_direct_field(file_y, &nods_[0], nods_.size(2), &edges_[0], edges_.size(2), &Y[0]);
  return 0;
}

int wave_constructor::load_feature_line(const char *filename) {
  return 0;
}

int wave_constructor::save_model_to_obj(const char *filename) const {
  return jtf::mesh::save_obj(filename, tris_, nods_);
}

int wave_constructor::save_wave_to_vtk(const char *filename) const {
  if ( f_.size(1) != 4 || f_.size(2) != nods_.size(2) )
    return __LINE__;
  matd_t data = f_(0, colon());
  return draw_vert_value_to_vtk(filename, &nods_[0], nods_.size(2), &tris_[0], tris_.size(2), &data[0]);
}

int wave_constructor::extract_edges() {
  if ( !e2c_.get() )
    return __LINE__;
  edges_.resize(2, e2c_->edges_.size());
#pragma omp parallel for
  for (size_t i = 0; i < e2c_->edges_.size(); ++i) {
    edges_(0, i) = e2c_->edges_[i].first;
    edges_(1, i) = e2c_->edges_[i].second;
  }
  e2c_.release();
  return 0;
}

int wave_constructor::build_frame_on_vert() {
  return 0;
}

int wave_constructor::init() {
  if ( extract_edges() )
    return __LINE__;
  f_ = zeros<double>(4, nods_.size(2));
  alphaIJ_ = zeros<double>(1, edges_.size(2));
  betaIJ_ = zeros<double>(1, edges_.size(2));
  cIJ_ = zeros<double>(4, edges_.size(2));
  cJI_ = zeros<double>(4, edges_.size(2));
  return 0;
}

/// @todo CHECK!
int wave_constructor::solve_phase_transition() {
  alpha_ij_.resize(1,  edge_.size(2));
  beta_ij_.resize(1, edge_.size(2));
  for (size_t i = 0; i < edge_.size(2); ++i) {
    const size_t I = edge_(0, i);
    const size_t J = edge_(1, i);
    alpha_ij_[i] = w*dot(nods_(colon(), I)-nods_(colon(), J), frameX);
    beta_ij_[i] = w*dot(nods_(colon(), I)-nods_(colon(), J), frameY);
  }
  c_ij_.resize(4, edge_.size(2));
  for (size_t i = 0; i < c_ij_.size(2); ++i) {
    c_ij_(0, i) = cos(alpha_ij_[i])*cos(beta_ij_[i]);
    c_ij_(1, i) = -cos(alpha_ij_[i])*sin(beta_ij_[i]);
    c_ij_(2, i) = -sin(alpha_ij_[i])*cos(beta_ij_[i]);
    c_ij_(3, i) = sin(alpha_ij_[i])*sin(beta_ij_[i]);
  }
  return 0;
}

int wave_constructor::prepare() {
  buff_.resize(3);
  buff_[0] = std::make_shared<wave_value_cons>(edges_, f_, cIJ_, cJI_, 1.0);
  buff_[1] = std::make_shared<modulus_cons>(f_, 0.15);
  buff_[2] = std::make_shared<phase_cons>(f_, 0.15);
  try {
    constraint_ = std::make_shared<constraint_t<double>>(buff_);
  } catch ( exception &e ) {
    cerr << "[exception] " << e.what() << endl;
    exit(EXIT_FAILURE);
  }
  cout << "[info] number of constraints: " << constraint_->Nf() << endl;
  return 0;
}

int wave_constructor::solve_wave_value() {
  // Gauss Newton
  const size_t xdim = constraint_->Nx();
  const size_t fdim = constraint_->Nf();
  Map<VectorXd> X(&f_[0], xdim);

  for (size_t iter = 0; iter < 20000; ++iter) {
    VectorXd cv = VectorXd::Zero(fdim); {
      constraint_->Val(&X[0], cv.data());
      if ( iter % 100 == 0 ) {
        cout << "\t@energy value: " << cv.squaredNorm() << endl;
      }
      if ( cv.lpNorm<Infinity>() < 1e-12 ) {
        cout << "[info] converged\n";
        break;
      }
    }
    SparseMatrix<double> J(fdim, xdim); {
      vector<Triplet<double>> trips;
      constraint_->Jac(&X[0], 0, &trips);
      J.reserve(trips.size());
      J.setFromTriplets(trips.begin(), trips.end());
    }
    SparseMatrix<double> LHS = J.transpose()*J;
    VectorXd rhs = -J.transpose()*cv;
    solver_.compute(LHS);
    ASSERT(solver_.info() == Success);
    VectorXd dx = solver_.solve(rhs);
    ASSERT(solver_.info() == Success);

    X += dx;
  }
  return 0;
}

//==============================================================================
void wave_constructor::test_wave_conditions() const {
  const double f1[4] = {1, 2, 3, 4};
  const double f2[4] = {1, -1, 1, -1};
  const double c[4] = {1, 1, 1, 1};
 {
    cout << "#TEST 1:\n";
    double val = 0;
    modulus_condition_(&val, f1);
    cout << "modulus: " << val << endl;
    phase_condition_(&val, f1);
    cout << "phase: " << val << endl << endl;
  }
  {
    cout << "#TEST 2:\n";
    double val = 0;
    modulus_condition_(&val, f2);
    cout << "modulus: " << val << endl;
    phase_condition_(&val, f2);
    cout << "phase: " << val << endl << endl;
  }
  {
    cout << "#TEST 3:\n";
    const double f[8] = {1, 2, 3, 4, 5, 6, 7, 9};
    double val = 0;
    wave_value_condition_(&val, f, c);
    cout << "transition: " << val << endl << endl;
  }
}

}