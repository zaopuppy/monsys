
numpy.repeat
-------------------------------------------------
    repeat(a, repeats, axis=None)
        Repeat elements of an array.
        
        Parameters
        ----------
        a : array_like
            Input array.
        repeats : {int, array of ints}
            The number of repetitions for each element.  `repeats` is broadcasted
            to fit the shape of the given axis.
        axis : int, optional
            The axis along which to repeat values.  By default, use the
            flattened input array, and return a flat output array.
        
        Returns
        -------
        repeated_array : ndarray
            Output array which has the same shape as `a`, except along
            the given axis.
        
        See Also
        --------
        tile : Tile an array.
        
        Examples
        --------
        >>> x = np.array([[1,2],[3,4]])
        >>> np.repeat(x, 2)
        array([1, 1, 2, 2, 3, 3, 4, 4])
        >>> np.repeat(x, 3, axis=1)
        array([[1, 1, 1, 2, 2, 2],
               [3, 3, 3, 4, 4, 4]])
        >>> np.repeat(x, [1, 2], axis=0)
        array([[1, 2],
               [3, 4],
               [3, 4]])

最重要的是第三个参数, 意思是对数据的第几个维度进行repeat, 如果是一个一维数组, 那么axis只能是0(flat), 如果是二维数组, 那么0表示对第一维进行repeat, 1表示对第二维, 以此类推.

    >>> a = numpy.array([ [ [1.1, 1.2], [2.1, 2.2], [3.1, 3.2] ], [[4.1, 4.2], [5.1, 5.2], [6.1, 6.2]] ])
    >>> numpy.repeat(a, [1, 2], axis = 0)
    array([[[ 1.1,  1.2],
            [ 2.1,  2.2],
            [ 3.1,  3.2]],

           [[ 4.1,  4.2],
            [ 5.1,  5.2],
            [ 6.1,  6.2]],

           [[ 4.1,  4.2],
            [ 5.1,  5.2],
            [ 6.1,  6.2]]])
    >>> numpy.repeat(a, [1, 2], axis = 2)
    array([[[ 1.1,  1.2,  1.2],
            [ 2.1,  2.2,  2.2],
            [ 3.1,  3.2,  3.2]],

           [[ 4.1,  4.2,  4.2],
            [ 5.1,  5.2,  5.2],
            [ 6.1,  6.2,  6.2]]])
    >>> 

