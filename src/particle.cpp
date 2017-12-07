#include "particle.hpp"

MPI_Datatype setup_particle_mpi_type() {

    MPI_Datatype new_type;

    const int num_blocks = 6;

    int blocklengths[] = {1, 1, DIM, DIM, DIM, DIM};
    MPI_Aint indices[6];
    indices[0] = (MPI_Aint) offsetof(struct Particle, id);
    indices[1] = (MPI_Aint) offsetof(struct Particle, m);
    indices[2] = (MPI_Aint) offsetof(struct Particle, x);
    indices[3] = (MPI_Aint) offsetof(struct Particle, v);
    indices[4] = (MPI_Aint) offsetof(struct Particle, F);
    indices[5] = (MPI_Aint) offsetof(struct Particle, F_old);

    MPI_Datatype old_types[] = {my_MPI_SIZE_T, my_MPI_REAL, my_MPI_REAL, my_MPI_REAL, my_MPI_REAL, my_MPI_REAL};

    MPI_Type_struct(num_blocks, blocklengths, indices, old_types, &new_type);

    MPI_Datatype resized_new_type;

    // padding
    MPI_Type_create_resized(new_type,
                            indices[0],
                            (MPI_Aint)sizeof(struct Particle),
                            &resized_new_type);
    MPI_Type_commit(&resized_new_type);

    // no longer needed
    MPI_Type_free(&new_type);

    return resized_new_type;
}

void free_particle_mpi_type(MPI_Datatype type) {
    MPI_Type_free(&type);
}

std::ostream& operator<<(std::ostream& os, Particle& p) {
    os << "{id: " << p.id << ", m: " << p.m << ", x: [";
    for (std::size_t d = 0; d < DIM; ++d) {
        os << p.x[d];
        if (d != DIM -1)
            os << ", ";
    }
    os << "], v: [";
    for (std::size_t d = 0; d < DIM; ++d) {
        os << p.v[d];
        if (d != DIM -1)
            os << ", ";
    }
    os << "], F: [";
    for (std::size_t d = 0; d < DIM; ++d) {
        os << p.F[d];
        if (d != DIM -1)
            os << ", ";
    }
    os << "], F_old: [";
    for (std::size_t d = 0; d < DIM; ++d) {
        os << p.F_old[d];
        if (d != DIM -1)
            os << ", ";
    }
    os << "]}";

    return os;
}