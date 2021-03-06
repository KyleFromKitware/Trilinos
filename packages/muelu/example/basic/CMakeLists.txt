INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../../test/unit_tests)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../../test/scaling)

#
# Executables
#

IF (${PACKAGE_NAME}_ENABLE_Tpetra AND ${PACKAGE_NAME}_ENABLE_Ifpack2 AND ${PACKAGE_NAME}_ENABLE_Amesos2)
  SET(${PACKAGE_NAME}_HAVE_TPETRA_SOLVER_STACK YES)
ENDIF()

IF (${PACKAGE_NAME}_ENABLE_Epetra AND ${PACKAGE_NAME}_ENABLE_EpetraExt AND ${PACKAGE_NAME}_ENABLE_Ifpack AND ${PACKAGE_NAME}_ENABLE_Amesos)
  SET(${PACKAGE_NAME}_HAVE_EPETRA_SOLVER_STACK YES)
ENDIF()

IF (${PACKAGE_NAME}_ENABLE_Galeri)
  IF (${PACKAGE_NAME}_HAVE_TPETRA_SOLVER_STACK)
    TRIBITS_ADD_EXECUTABLE(
      Simple
      SOURCES Simple.cpp
      COMM serial mpi
      )

    TRIBITS_ADD_TEST(
        Simple
        NAME "SimpleTpetra"
        ARGS "--linAlgebra=Tpetra --xml=simple.xml"
        NUM_MPI_PROCS 4
        COMM mpi # HAVE_MPI required
        )

      TRIBITS_ADD_TEST(
        Simple
        NAME "SimpleTpetraYaml"
        ARGS "--linAlgebra=Tpetra --yaml=simple.yaml"
        NUM_MPI_PROCS 4
        COMM mpi # HAVE_MPI required
        )

    TRIBITS_COPY_FILES_TO_BINARY_DIR(Simple_cp
      SOURCE_FILES simple.xml simple.yaml
     )
  ENDIF()
ENDIF()
