// The C API tests try to follow tests in tstUserApplication
#include <DTK_C_API.h>

#include <mpi.h>

#include <assert.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//---------------------------------------------------------------------------//
// User class
//---------------------------------------------------------------------------//
typedef struct UserTestClass
{
    unsigned _space_dim;
    unsigned _size_1;
    unsigned _size_2;
    unsigned _offset;
    const char *_boundary_name;
    const char *_field_name;
    double *_data;
} UserTestClass;

//---------------------------------------------------------------------------//
// User functions.
//---------------------------------------------------------------------------//
// Get the size parameters for building a node list.
void node_list_size( void *user_data, unsigned *space_dim,
                     size_t *local_num_nodes, bool *has_ghosts )
{
    UserTestClass *u = (UserTestClass *)user_data;

    *space_dim = u->_space_dim;
    *local_num_nodes = u->_size_1;
    *has_ghosts = true;
}

//---------------------------------------------------------------------------//
// Get the data for a node list.
void node_list_data( void *user_data, Coordinate *coordinates,
                     bool *is_ghost_node )
{
    UserTestClass *u = (UserTestClass *)user_data;

    for ( int i = 0; i < u->_size_1; ++i )
    {
        for ( int j = 0; j < u->_space_dim; j++ )
            coordinates[j * u->_size_1 + i] = i + j + u->_offset;
        is_ghost_node[i] = true;
    }
}

//---------------------------------------------------------------------------//
// Get the size parameters for building a bounding volume list.
void bounding_volume_list_size( void *user_data, unsigned *space_dim,
                                size_t *local_num_volumes, bool *has_ghosts )
{
    UserTestClass *u = (UserTestClass *)user_data;

    *space_dim = u->_space_dim;
    *local_num_volumes = u->_size_1;
    *has_ghosts = true;
}

//---------------------------------------------------------------------------//
// Get the data for a bounding volume list.
void bounding_volume_list_data( void *user_data, Coordinate *bounding_volumes,
                                bool *is_ghost_volume )
{
    UserTestClass *u = (UserTestClass *)user_data;

    for ( size_t v = 0; v < u->_size_1; v++ )
    {
        for ( unsigned d = 0; d < u->_space_dim; ++d )
        {
            for ( unsigned h = 0; h < 2; ++h )
            {
                unsigned index =
                    u->_size_1 * u->_space_dim * h + u->_size_1 * d + v;
                bounding_volumes[index] = v + d + h + u->_offset;
            }
        }
        is_ghost_volume[v] = true;
    }
}

//---------------------------------------------------------------------------//
// Get the size parameters for building a polyhedron list.
void polyhedron_list_size( void *user_data, unsigned *space_dim,
                           size_t *local_num_nodes, size_t *local_num_faces,
                           size_t *total_nodes_per_face,
                           size_t *local_num_cells,
                           size_t *total_faces_per_cell, bool *has_ghosts )
{
    UserTestClass *u = (UserTestClass *)user_data;

    *space_dim = u->_space_dim;
    *local_num_nodes = u->_size_1;
    *local_num_faces = u->_size_1;
    *total_nodes_per_face = u->_size_1;
    *local_num_cells = u->_size_1;
    *total_faces_per_cell = u->_size_1;
    *has_ghosts = true;
}

//---------------------------------------------------------------------------//
// Get the data for a polyhedron list.
void polyhedron_list_data( void *user_data, Coordinate *coordinates,
                           LocalOrdinal *faces, unsigned *nodes_per_face,
                           LocalOrdinal *cells, unsigned *faces_per_cell,
                           int *face_orientation, bool *is_ghost_cell )
{
    UserTestClass *u = (UserTestClass *)user_data;

    for ( size_t n = 0; n < u->_size_1; n++ )
    {
        for ( unsigned d = 0; d < u->_space_dim; ++d )
        {
            coordinates[u->_size_1 * d + n] = n + d + u->_offset;
        }
        faces[n] = n + u->_offset;
        nodes_per_face[n] = n + u->_offset;
        cells[n] = n + u->_offset;
        faces_per_cell[n] = n + u->_offset;
        face_orientation[n] = 1;
        is_ghost_cell[n] = true;
    }
}

//---------------------------------------------------------------------------//
// Get the size parameters for building a cell list with a single
// topology.
void cell_list_size( void *user_data, unsigned *space_dim,
                     size_t *local_num_nodes, size_t *local_num_cells,
                     unsigned *nodes_per_cell, bool *has_ghosts )
{
    UserTestClass *u = (UserTestClass *)user_data;

    *space_dim = u->_space_dim;
    *local_num_nodes = u->_size_1;
    *local_num_cells = u->_size_1;
    *nodes_per_cell = u->_size_2;
    *has_ghosts = true;
}

//---------------------------------------------------------------------------//
// Get the data for a single topology cell list.
void cell_list_data( void *user_data, Coordinate *coordinates,
                     LocalOrdinal *cells, bool *is_ghost_cell,
                     char *cell_topology )
{
    UserTestClass *u = (UserTestClass *)user_data;

    for ( size_t n = 0; n < u->_size_1; n++ )
    {
        for ( unsigned d = 0; d < u->_space_dim; ++d )
            coordinates[u->_size_1 * d + n] = n + d + u->_offset;
        for ( unsigned v = 0; v < u->_size_2; ++v )
            cells[v * u->_size_1 + n] = n + v + u->_offset;
        is_ghost_cell[n] = true;
    }

    strcpy( cell_topology, "unit_test_topology" );
}

//---------------------------------------------------------------------------//
// Get the size parameters for building a cell list with mixed
// topologies.
void mixed_topology_cell_list_size( void *user_data, unsigned *space_dim,
                                    size_t *local_num_nodes,
                                    size_t *local_num_cells,
                                    size_t *total_nodes_per_cell,
                                    bool *has_ghosts )
{
    UserTestClass *u = (UserTestClass *)user_data;

    *space_dim = u->_space_dim;
    *local_num_nodes = u->_size_1;
    *local_num_cells = u->_size_1;
    *total_nodes_per_cell = u->_size_1;
    *has_ghosts = true;
}

//---------------------------------------------------------------------------//
// Get the data for a mixed topology cell list.
void mixed_topology_cell_list_data( void *user_data, Coordinate *coordinates,
                                    LocalOrdinal *cells,
                                    unsigned *cell_topology_ids,
                                    bool *is_ghost_cell,
                                    char **cell_topologies )
{
    UserTestClass *u = (UserTestClass *)user_data;

    for ( size_t n = 0; n < u->_size_1; n++ )
    {
        for ( unsigned d = 0; d < u->_space_dim; ++d )
            coordinates[u->_size_1 * d + n] = n + d + u->_offset;
        cells[n] = n + u->_offset;
        cell_topology_ids[n] = 0;
        is_ghost_cell[n] = true;
    }

    strcpy( cell_topologies[0], "unit_test_topology" );
}

//---------------------------------------------------------------------------//
// Get the size parameters for a boundary.
void boundary_size( void *user_data, const char *boundary_name,
                    size_t *local_num_faces )
{
    UserTestClass *u = (UserTestClass *)user_data;

    // Here one could do actions depening on the name, but in the tests we
    // simply ignore it
    (void)boundary_name;
    *local_num_faces = u->_size_1;
}

//---------------------------------------------------------------------------//
// Get the data for a boundary.
void boundary_data( void *user_data, const char *boundary_name,
                    LocalOrdinal *boundary_cells,
                    unsigned *cell_faces_on_boundary )
{
    UserTestClass *u = (UserTestClass *)user_data;

    // Here one could do actions depening on the name, but in the tests we
    // simply ignore it
    (void)boundary_name;

    for ( size_t n = 0; n < u->_size_1; n++ )
    {
        boundary_cells[n] = n + u->_offset;
        cell_faces_on_boundary[n] = n + u->_offset;
    }
}

//---------------------------------------------------------------------------//
// Get the size parameters for a degree-of-freedom id map with a single
// number of dofs per object.
void dof_map_size( void *user_data, size_t *local_num_dofs,
                   size_t *local_num_objects, unsigned *dofs_per_object )
{
    UserTestClass *u = (UserTestClass *)user_data;

    *local_num_dofs = u->_size_1;
    *local_num_objects = u->_size_1;
    *dofs_per_object = u->_size_2;
}

//---------------------------------------------------------------------------//
// Get the data for a degree-of-freedom id map with a single number of
// dofs per object.
void dof_map_data( void *user_data, GlobalOrdinal *global_dof_ids,
                   LocalOrdinal *object_dof_ids, char *discretization_type )
{
    UserTestClass *u = (UserTestClass *)user_data;

    for ( size_t n = 0; n < u->_size_1; n++ )
    {
        global_dof_ids[n] = n + u->_offset;
        for ( unsigned d = 0; d < u->_size_2; ++d )
            object_dof_ids[u->_size_1 * d + n] = n + d + u->_offset;
    }

    strcpy( discretization_type, "unit_test_discretization" );
}

//---------------------------------------------------------------------------//
// Get the size parameters for a degree-of-freedom id map with a
// multiple number of dofs per object (e.g. mixed topology cell lists or
// polyhedron lists).
void mixed_topology_dof_map_size( void *user_data, size_t *local_num_dofs,
                                  size_t *local_num_objects,
                                  size_t *total_dofs_per_object )
{
    UserTestClass *u = (UserTestClass *)user_data;

    *local_num_dofs = u->_size_1;
    *local_num_objects = u->_size_1;
    *total_dofs_per_object = u->_size_1;
}

//---------------------------------------------------------------------------//
// Get the data for a multiple object degree-of-freedom id map
// (e.g. mixed topology cell lists or polyhedron lists).
void mixed_topology_dof_map_data( void *user_data,
                                  GlobalOrdinal *global_dof_ids,
                                  LocalOrdinal *object_dof_ids,
                                  unsigned *dofs_per_object,
                                  char *discretization_type )
{
    UserTestClass *u = (UserTestClass *)user_data;

    for ( size_t n = 0; n < u->_size_1; n++ )
    {
        global_dof_ids[n] = n + u->_offset;
        object_dof_ids[n] = n + u->_offset;
        dofs_per_object[n] = u->_size_2;
    }

    strcpy( discretization_type, "unit_test_discretization" );
}

//---------------------------------------------------------------------------//
// Get the size parameters for a field. Field must be of size
// local_num_dofs in the associated dof_id_map.
void field_size( void *user_data, const char *field_name,
                 unsigned *field_dimension, size_t *local_num_dofs )
{
    UserTestClass *u = (UserTestClass *)user_data;

    // Here one could do actions depening on the name, but in the tests we
    // simply ignore it
    (void)field_name;

    *field_dimension = u->_space_dim;
    *local_num_dofs = u->_size_1;
}

//---------------------------------------------------------------------------//
// Pull data from application into a field.
void pull_field_data( void *user_data, const char *field_name,
                      double *field_dofs )

{
    UserTestClass *u = (UserTestClass *)user_data;

    // Here one could do actions depening on the name, but in the tests we
    // simply ignore it
    (void)field_name;

    for ( size_t n = 0; n < u->_size_1; n++ )
    {
        for ( unsigned d = 0; d < u->_space_dim; ++d )
            field_dofs[d * u->_size_1 + n] = u->_data[d * u->_size_1 + n];
    }
}

//---------------------------------------------------------------------------//
// Push data from a field into the application.
void push_field_data( void *user_data, const char *field_name,
                      const double *field_dofs )

{
    UserTestClass *u = (UserTestClass *)user_data;

    // Here one could do actions depening on the name, but in the tests we
    // simply ignore it
    (void)field_name;

    for ( size_t n = 0; n < u->_size_1; n++ )
    {
        for ( unsigned d = 0; d < u->_space_dim; ++d )
            u->_data[d * u->_size_1 + n] = field_dofs[d * u->_size_1 + n];
    }
}

//---------------------------------------------------------------------------//
// Evaluate a field at a given set of points in a given set of objects.
void evaluate_field( void *user_data, const char *field_name,
                     const Coordinate *evaluation_points,
                     const LocalOrdinal *object_ids, double *values )
{
    UserTestClass *u = (UserTestClass *)user_data;

    // Here one could do actions depening on the name, but in the tests we
    // simply ignore it
    (void)field_name;

    for ( size_t n = 0; n < u->_size_1; n++ )
    {
        for ( unsigned d = 0; d < u->_space_dim; ++d )
            values[d * u->_size_1 + n] =
                evaluation_points[d * u->_size_1 + n] + object_ids[n];
    }
}

//---------------------------------------------------------------------------//
// Tests
//---------------------------------------------------------------------------//
extern int check_registry( const char *test_name,
                           DTK_UserApplicationHandle handle, UserTestClass u );

int test_node_list( DTK_UserApplicationHandle dtk_handle, UserTestClass u )
{
    DTK_set_function( dtk_handle, DTK_NODE_LIST_SIZE_FUNCTION, node_list_size,
                      &u );
    DTK_set_function( dtk_handle, DTK_NODE_LIST_DATA_FUNCTION, node_list_data,
                      &u );

    return check_registry( "test_node_list", dtk_handle, u );
}

int test_bounding_volume_list( DTK_UserApplicationHandle dtk_handle,
                               UserTestClass u )
{
    DTK_set_function( dtk_handle, DTK_BOUNDING_VOLUME_LIST_SIZE_FUNCTION,
                      bounding_volume_list_size, &u );
    DTK_set_function( dtk_handle, DTK_BOUNDING_VOLUME_LIST_DATA_FUNCTION,
                      bounding_volume_list_data, &u );

    return check_registry( "test_bounding_volume_list", dtk_handle, u );
}

int test_polyhedron_list( DTK_UserApplicationHandle dtk_handle,
                          UserTestClass u )
{
    DTK_set_function( dtk_handle, DTK_POLYHEDRON_LIST_SIZE_FUNCTION,
                      polyhedron_list_size, &u );
    DTK_set_function( dtk_handle, DTK_POLYHEDRON_LIST_DATA_FUNCTION,
                      polyhedron_list_data, &u );

    return check_registry( "test_polyhedron_list", dtk_handle, u );
}

int test_single_topology_cell( DTK_UserApplicationHandle dtk_handle,
                               UserTestClass u )
{
    DTK_set_function( dtk_handle, DTK_CELL_LIST_SIZE_FUNCTION, cell_list_size,
                      &u );
    DTK_set_function( dtk_handle, DTK_CELL_LIST_DATA_FUNCTION, cell_list_data,
                      &u );

    return check_registry( "test_single_topology_cell", dtk_handle, u );
}

int test_multiple_topology_cell( DTK_UserApplicationHandle dtk_handle,
                                 UserTestClass u )
{
    DTK_set_function( dtk_handle, DTK_MIXED_TOPOLOGY_CELL_LIST_SIZE_FUNCTION,
                      mixed_topology_cell_list_size, &u );
    DTK_set_function( dtk_handle, DTK_MIXED_TOPOLOGY_CELL_LIST_DATA_FUNCTION,
                      mixed_topology_cell_list_data, &u );

    return check_registry( "test_multiple_topology_cell", dtk_handle, u );
}

int test_boundary( DTK_UserApplicationHandle dtk_handle, UserTestClass u )
{
    DTK_set_function( dtk_handle, DTK_BOUNDARY_SIZE_FUNCTION, boundary_size,
                      &u );
    DTK_set_function( dtk_handle, DTK_BOUNDARY_DATA_FUNCTION, boundary_data,
                      &u );
    DTK_set_function( dtk_handle, DTK_CELL_LIST_SIZE_FUNCTION, cell_list_size,
                      &u );
    DTK_set_function( dtk_handle, DTK_CELL_LIST_DATA_FUNCTION, cell_list_data,
                      &u );
    DTK_set_function( dtk_handle, DTK_POLYHEDRON_LIST_SIZE_FUNCTION,
                      polyhedron_list_size, &u );
    DTK_set_function( dtk_handle, DTK_POLYHEDRON_LIST_DATA_FUNCTION,
                      polyhedron_list_data, &u );

    return check_registry( "test_boundary", dtk_handle, u );
}

int test_single_topology_dof( DTK_UserApplicationHandle dtk_handle,
                              UserTestClass u )
{
    DTK_set_function( dtk_handle, DTK_DOF_MAP_SIZE_FUNCTION, dof_map_size, &u );
    DTK_set_function( dtk_handle, DTK_DOF_MAP_DATA_FUNCTION, dof_map_data, &u );

    return check_registry( "test_single_topology_dof", dtk_handle, u );
}

int test_multiple_topology_dof( DTK_UserApplicationHandle dtk_handle,
                                UserTestClass u )
{
    DTK_set_function( dtk_handle, DTK_MIXED_TOPOLOGY_DOF_MAP_SIZE_FUNCTION,
                      mixed_topology_dof_map_size, &u );
    DTK_set_function( dtk_handle, DTK_MIXED_TOPOLOGY_DOF_MAP_DATA_FUNCTION,
                      mixed_topology_dof_map_data, &u );

    return check_registry( "test_multiple_topology_dof", dtk_handle, u );
}

int test_field_push_pull( DTK_UserApplicationHandle dtk_handle,
                          UserTestClass u )
{
    DTK_set_function( dtk_handle, DTK_FIELD_SIZE_FUNCTION, field_size, &u );
    DTK_set_function( dtk_handle, DTK_PULL_FIELD_DATA_FUNCTION, pull_field_data,
                      &u );
    DTK_set_function( dtk_handle, DTK_PUSH_FIELD_DATA_FUNCTION, push_field_data,
                      &u );

    return check_registry( "test_field_push_pull", dtk_handle, u );
}

int test_field_eval( DTK_UserApplicationHandle dtk_handle, UserTestClass u )
{
    DTK_set_function( dtk_handle, DTK_FIELD_SIZE_FUNCTION, field_size, &u );
    DTK_set_function( dtk_handle, DTK_EVALUATE_FIELD_FUNCTION, evaluate_field,
                      &u );

    return check_registry( "test_field_eval", dtk_handle, u );
}

int test_missing_function( DTK_UserApplicationHandle dtk_handle,
                           UserTestClass u )
{
    DTK_set_function( dtk_handle, DTK_NODE_LIST_SIZE_FUNCTION, node_list_size,
                      &u );

    return check_registry( "test_missing_function", dtk_handle, u );
}

int test_too_many_functions( DTK_UserApplicationHandle dtk_handle,
                             UserTestClass u )
{
    DTK_set_function( dtk_handle, DTK_CELL_LIST_SIZE_FUNCTION, cell_list_size,
                      &u );
    DTK_set_function( dtk_handle, DTK_CELL_LIST_DATA_FUNCTION, cell_list_data,
                      &u );
    DTK_set_function( dtk_handle, DTK_DOF_MAP_SIZE_FUNCTION, dof_map_size, &u );
    DTK_set_function( dtk_handle, DTK_DOF_MAP_DATA_FUNCTION, dof_map_data, &u );
    DTK_set_function( dtk_handle, DTK_MIXED_TOPOLOGY_CELL_LIST_SIZE_FUNCTION,
                      mixed_topology_cell_list_size, &u );
    DTK_set_function( dtk_handle, DTK_MIXED_TOPOLOGY_CELL_LIST_DATA_FUNCTION,
                      mixed_topology_cell_list_data, &u );
    DTK_set_function( dtk_handle, DTK_MIXED_TOPOLOGY_DOF_MAP_SIZE_FUNCTION,
                      mixed_topology_dof_map_size, &u );
    DTK_set_function( dtk_handle, DTK_MIXED_TOPOLOGY_DOF_MAP_DATA_FUNCTION,
                      mixed_topology_dof_map_data, &u );

    return check_registry( "test_too_many_functions", dtk_handle, u );
}

int main( int argc, char *argv[] )
{
    MPI_Init( &argc, &argv );

    MPI_Comm comm = MPI_COMM_WORLD;
    int comm_rank;
    MPI_Comm_rank( comm, &comm_rank );

    DTK_ExecutionSpace exec_space = DTK_SERIAL;

    int opt;
    while ( ( opt = getopt( argc, argv, "s:h" ) ) != -1 )
    {
        switch ( opt )
        {
        case 's':
            if ( !strcmp( optarg, "serial" ) )
                exec_space = DTK_SERIAL;
            else if ( !strcmp( optarg, "openmp" ) )
                exec_space = DTK_OPENMP;
            else if ( !strcmp( optarg, "cuda" ) )
                exec_space = DTK_CUDA;
            else
            {
                printf( "Unknown execution space\n" );
                return EXIT_FAILURE;
            }
            break;

        case 'h':
            printf( "Usage: %s [-s <serial|openmp|cuda>] [-h]\n", argv[0] );
            return EXIT_FAILURE;
        }
    }

    if ( !comm_rank )
    {
        printf( "DTK version: %s\n", DTK_version() );
        printf( "DTK hash: %s\n", DTK_git_commit_hash() );
    }

    UserTestClass u;
    // NOTE: this parameters MUST match parameters in tstUserApplication.cpp
    u._space_dim = 3;
    u._size_1 = 100;
    u._size_2 = 5;
    u._offset = 8;
    u._boundary_name = "unit_test_boundary";
    u._field_name = "test_field";
    u._data = (double *)malloc( u._size_1 * u._space_dim * sizeof( double ) );

    int rv = 0;

    DTK_initialize_cmd( &argc, &argv );
    rv |= ( DTK_is_initialized() ? 0 : 1 );

    {
        DTK_UserApplicationHandle dtk_handle = DTK_create( exec_space );
        rv |= ( DTK_is_valid( dtk_handle ) ? 0 : 1 );
        DTK_destroy( dtk_handle );
        rv |= ( DTK_is_valid( dtk_handle ) ? 1 : 0 );
    }
    {
        DTK_UserApplicationHandle dtk_handle;
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
        rv |= ( DTK_is_valid( dtk_handle ) ? 1 : 0 );
        DTK_destroy( dtk_handle );
    }
    {
        DTK_UserApplicationHandle dtk_handle = DTK_create( exec_space );
        rv |= test_node_list( dtk_handle, u );
        DTK_destroy( dtk_handle );
    }
    {
        DTK_UserApplicationHandle dtk_handle = DTK_create( exec_space );
        rv |= test_bounding_volume_list( dtk_handle, u );
        DTK_destroy( dtk_handle );
    }
    {
        DTK_UserApplicationHandle dtk_handle = DTK_create( exec_space );
        rv |= test_polyhedron_list( dtk_handle, u );
        DTK_destroy( dtk_handle );
    }
    {
        DTK_UserApplicationHandle dtk_handle = DTK_create( exec_space );
        rv |= test_single_topology_cell( dtk_handle, u );
        DTK_destroy( dtk_handle );
    }
    {
        DTK_UserApplicationHandle dtk_handle = DTK_create( exec_space );
        rv |= test_multiple_topology_cell( dtk_handle, u );
        DTK_destroy( dtk_handle );
    }
    {
        DTK_UserApplicationHandle dtk_handle = DTK_create( exec_space );
        rv |= test_boundary( dtk_handle, u );
        DTK_destroy( dtk_handle );
    }
    {
        DTK_UserApplicationHandle dtk_handle = DTK_create( exec_space );
        rv |= test_single_topology_dof( dtk_handle, u );
        DTK_destroy( dtk_handle );
    }
    {
        DTK_UserApplicationHandle dtk_handle = DTK_create( exec_space );
        rv |= test_multiple_topology_dof( dtk_handle, u );
        DTK_destroy( dtk_handle );
    }
    {
        DTK_UserApplicationHandle dtk_handle = DTK_create( exec_space );
        rv |= test_field_push_pull( dtk_handle, u );
        DTK_destroy( dtk_handle );
    }
    {
        DTK_UserApplicationHandle dtk_handle = DTK_create( exec_space );
        rv |= test_field_eval( dtk_handle, u );
        DTK_destroy( dtk_handle );
    }
    {
        DTK_UserApplicationHandle dtk_handle = DTK_create( exec_space );
        rv |= test_missing_function( dtk_handle, u );
        DTK_destroy( dtk_handle );
    }
    {
        DTK_UserApplicationHandle dtk_handle = DTK_create( exec_space );
        rv |= test_too_many_functions( dtk_handle, u );
        DTK_destroy( dtk_handle );
    }

    DTK_finalize();

    if ( !comm_rank )
    {
        if ( rv == 0 )
            printf( "End Result: TEST PASSED\n" );
        else
            printf( "End Result: TEST FAILED\n" );
    }

    free( u._data );

    MPI_Finalize();

    return EXIT_SUCCESS;
}
